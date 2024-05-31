#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <libgen.h>

#include "./headers/datastrcts.h"
#include "./headers/lodg.h"
#include "./headers/config.h"
#include "./headers/util.h"
#include "./headers/gather.h"
#include "./headers/metstor.h"
#include "./headers/globs.h"

#ifdef ENABLE_MQTT
#include "./exporters/headers/mqtt.h"
#endif

#define GATHERING_LOOP_PERIOD_SEC 1
#define OFLLOAD_LOOP_PERIOD_SEC 60
#define MAIN_THREAD_LOOP_PERIOD_SEC 300

char* version = "1.0.0";
pthread_mutex_t print_lock;
bool debug = false;
char* exporter_config;
char* exporter;
struct monitoring_point* monitoring_points;
uint32_t gathering_iteration = 0;
uint32_t offload_iteration = 0;
uint32_t main_iteration = 0;
time_t current_time_s = 1;

bool timetodie = false;
uint8_t exitcodetodiewith = 0;

int main_wrapped(int argc, char** argv){
    char* config_filepath;
    uint64_t freemem = get_free_memory();

    // Parse cmdline arguments
    for (int i = 0; i < argc; i++){
        if(strcmp(argv[i], "-v") == 0){
            printver();
            exit(EXIT_SUCCESS);
        }
        if(strcmp(argv[i], "-h") == 0){
            printver();
            print_help();
            exit(EXIT_SUCCESS);
        }
        if(strcmp(argv[i], "-c") == 0){
            config_filepath = argv[i+1];
            i++;
        }
        if(strcmp(argv[i], "-o") == 0){
            exporter_config = argv[i+1];
            i++;
        }
        if(strcmp(argv[i], "-d") == 0){
            debug = true;
            T_printdbg("Debug mode toggled\n");
        }
        if(strcmp(argv[i], "-e") == 0){
            exporter = argv[i+1];
            i++;
        }
    }

    if(config_filepath == NULL || exporter_config == NULL || exporter == NULL){
        print_help();
        exit(EXIT_FAILURE);
    }

    T_printf("lodg data gatherer. Starting...\n");
    T_printf("Free memory: %ldK\n", freemem / 1024);
    bool config_result = initialize(config_filepath);

    if (config_result == false){
        T_printf("Could not configure.\n");
        exit(EXIT_FAILURE);
    }
    start_threads();
    return(EXIT_FAILURE);
}

void printver(){
    T_printf("lodg version %s built on %s.\n", version, __DATE__);
}

void start_threads(){
    int thr_error;
    pthread_t thread_ids[3];

    T_printf("Starting threads...\n");

    thr_error = pthread_create(&(thread_ids[0]), NULL, &gathering_loop, NULL); 
    pthread_setname_np(thread_ids[0], "Gathering");
    if (thr_error != 0) {
        T_printf("\nGathering thread creation failed:[%s]\n", strerror(thr_error));
    }
    
    // File exporter needs an offload thread
    thr_error = pthread_create(&(thread_ids[1]), NULL, &offload_loop, NULL); 
    pthread_setname_np(thread_ids[1], "Metric offload");
    if (thr_error != 0) {
        T_printf("\nOffload thread creation failed:[%s]\n", strerror(thr_error));
    }

    #ifdef ENABLE_MQTT
    if(strcmp(exporter, "MQTT") == 0){
        thr_error = pthread_create(&(thread_ids[2]), NULL, &mqtt_loop, NULL); 
        pthread_setname_np(thread_ids[2], "MQTT");
        if (thr_error != 0) {
            T_printf("\nMQTT thread creation failed:[%s]\n", strerror(thr_error));
        }
    }
    #endif
    T_printf("Threads started. Merging main thread with gathering thread.\n");

    main_loop();
}

void main_loop(){
        T_printf("OT: Main loop started.\n");
        while(true){
        if(!timetodie){
            main_iteration++;
            T_printdbg("MT: Main thread iteration #%d\n", main_iteration);

            #ifdef __GLIBC__
            T_printdbg("MT: Memory statistics:\n");
            print_memory_stats();
            T_printdbg("MT: Freeing excess heap memory...\n");
            free_unused_heap();
            #endif

            sleep(MAIN_THREAD_LOOP_PERIOD_SEC);
        } else {
            T_printf("MT: Exiting with code %d\n", exitcodetodiewith);
            exit(exitcodetodiewith);
        }
    }
}

void* offload_loop(){
    sleep(1);
    uint32_t metrics_offloaded;
    T_printf("OT: Offload loop started.\n");
    while(true){
        sleep(OFLLOAD_LOOP_PERIOD_SEC);
        offload_iteration++;
        metrics_offloaded = offload_metrics();
        T_printdbg("OT: Offloaded %d metrics.\n", metrics_offloaded);
        if(timetodie){
            exit(exitcodetodiewith);
        }
    }
}

void* gathering_loop(){
    sleep(1);
    struct timespec tstart={0,0}, tend={0,0};
    long double iteration_time = 0;
    T_printf("GT: Gathering loop started.\n");
    while(true){
        gathering_iteration++;
        clock_gettime(CLOCK_MONOTONIC, &tstart);

        T_printdbg("GT: Iteration: #%d started.\n", gathering_iteration);
        gathering_loop_iteration();

        clock_gettime(CLOCK_MONOTONIC, &tend);
        iteration_time = ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec);
        T_printdbg("GT: Iteration took %Lfs\n", iteration_time);

        sleep(GATHERING_LOOP_PERIOD_SEC);
    }
}

void gathering_loop_iteration(){
    struct monitoring_point* current_point = monitoring_points;
    struct monitoring_data_entry* current_data_entry;
    do {
        time(&current_time_s);
        if(current_point->last_gathered + current_point->gathering_interval_sec <= current_time_s
            || current_point->last_gathered == 0){
            current_data_entry = gather_metric(current_point);
            push_mon_entry(current_data_entry);
        }
        current_point = current_point->next;
    } while (current_point != NULL);
}

bool initialize(char* cfg_filepath){
    T_printf("Config file path is: %s\n", cfg_filepath);
    T_printf("Loading config...");
    struct config* configuration = get_config(cfg_filepath);

    if (configuration == NULL){
        T_printf("Empty configuration!\n");
        return false;
    }

    monitoring_points = configure_monitoring_points(configuration);
    T_printf(" Done.\n");
    
    struct monitoring_point* temp = monitoring_points;
    T_printf("Monitoring points:\n");
    do {
        T_printf("\tID: %d\n", temp->device_id);
        T_printf("\tDevice type: %s\n", temp->device_type);
        T_printf("\tDevice name: %s\n", temp->device_name);
        T_printf("\tDevice config: %s\n", temp->cfg);
        T_printf("\tGathering interval: %ds\n", temp->gathering_interval_sec);
        T_printf("\n");
        temp = temp->next;
    } while (temp != NULL);
    
    T_printf("Initializing exporter...\n");
    char* error = initialize_exporter();
    if (error != NULL){
        T_printf("Error initializing exporter: %s\n", error);
        exit(EXIT_FAILURE);
    }

    return true;
}

char* initialize_exporter(){
    if(strcmp(exporter, "FILE") == 0){
        char* dirname_ptr = malloc(sizeof(char) * strlen(exporter_config));
        strcpy(dirname_ptr, exporter_config);
        dirname(dirname_ptr);
        if(access(dirname_ptr, W_OK) == 0) {
            free(dirname_ptr);
            return NULL;
        } else {
            free(dirname_ptr);
            return "Filepath not accessible!";   
        }
    }

#ifdef ENABLE_MQTT
    if(strcmp(exporter, "MQTT") == 0){
        return initialize_mqtt(exporter_config, monitoring_points);
    }
#endif

    return "Unknown exporter.";
}