#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../headers/datastrcts.h"
#include "../headers/meteo.h"
#include "../headers/config.h"
#include "../headers/util.h"
#include "../headers/gather.h"
#include "../headers/metstor.h"
#include "../headers/globs.h"

bool debug = false;
char* offload_location;
struct monitoring_point* monitoring_points;
uint32_t gathering_iteration = 0;
uint32_t offload_iteration = 0;
time_t current_time_s = 1;

bool timetodie = false;
uint8_t exitcodetodiewith = 0;

int main_wrapped(int argc, char** argv){
    char* config_filepath;
    uint64_t freemem = get_free_memory();

    // Parse cmdline arguments
    for (int i = 0; i < argc; i++){
        if(strcmp(argv[i], "-h") == 0){
            print_help();
            exit(EXIT_SUCCESS);
        }
        if(strcmp(argv[i], "-c") == 0){
            config_filepath = argv[i+1];
            i++;
        }
        if(strcmp(argv[i], "-o") == 0){
            offload_location = argv[i+1];
            i++;
        }
        if(strcmp(argv[i], "-d") == 0){
            debug = true;
            printdbg("Debug mode toggled\n");
        }
    }

    if(config_filepath == NULL || offload_location == NULL){
        print_help();
        exit(EXIT_FAILURE);
    }

    printf("Meteo data gatherer. Starting...\n");
    printf("Free memory: %ldK\n", freemem / 1024);
    bool config_result = initialize(config_filepath);

    if (config_result == false){
        printf("Could not configure.\n");
        exit(EXIT_FAILURE);
    }
    start_threads();
    return(EXIT_FAILURE);
}

void start_threads(){
    int thr_error;
    pthread_t thread_ids[2];

    printf("Starting threads...\n");

    thr_error = pthread_create(&(thread_ids[0]), NULL, &gathering_loop, NULL); 
    
    if (thr_error != 0) {
        printf("\nGathering thread creation failed:[%s]\n", strerror(thr_error));
    }
    
    thr_error = pthread_create(&(thread_ids[1]), NULL, &offload_loop, NULL); 

    if (thr_error != 0) {
        printf("\nGathering thread creation failed:[%s]\n", strerror(thr_error));
    }

    printf("Threads started. Merging main thread with gathering thread.\n");
    
    pthread_join(thread_ids[0], NULL);
    printf("ERROR: Gathering thread died. Waiting for the next metric offload and exiting!\n");
    timetodie = true;
    exitcodetodiewith = 1;
    pthread_join(thread_ids[1], NULL);
}

void* offload_loop(){
    uint32_t metrics_offloaded;
    printf("OT: Offload loop started.\n");
    while(true){
        sleep(15);
        offload_iteration++;
        metrics_offloaded = offload_metrics();
        printdbg("OT: Offloaded %d metrics.\n", metrics_offloaded);
        if(timetodie){
            exit(exitcodetodiewith);
        }
    }
}

void* gathering_loop(){
    struct timespec tstart={0,0}, tend={0,0};
    long double iteration_time = 0;
    printf("GT: Gathering loop started.\n");
    while(true){
        gathering_iteration++;
        clock_gettime(CLOCK_MONOTONIC, &tstart);

        printdbg("GT: Iteration: #%d started.\n", gathering_iteration);
        gathering_loop_iteration();

        clock_gettime(CLOCK_MONOTONIC, &tend);
        iteration_time = ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec);
        printdbg("GT: Iteration took %Lfs\n", iteration_time);

        sleep(1);
    }
}

void gathering_loop_iteration(){
    struct monitoring_point* current_point = monitoring_points;
    struct monitoring_data_entry* current_data_entry;
    do {
        time(&current_time_s);
        if(current_point->last_gathered + current_point->gathering_interval_sec >= current_time_s
            || current_point->last_gathered == 0){
            current_data_entry = gather_metric(current_point);
            push_mon_entry(current_data_entry);
        }
        current_point = current_point->next;
    } while (current_point != NULL);
}

bool initialize(char* cfg_filepath){
    printf("Config file path is: %s\n", cfg_filepath);
    printf("Loading config...");
    struct config* configuration = get_config(cfg_filepath);

    if (configuration == NULL){
        printf("Empty configuration!\n");
        return false;
    }

    monitoring_points = configure_monitoring_points(configuration);
    printf(" Done.\n");
    
    struct monitoring_point* temp = monitoring_points;
    printf("Monitoring points:\n");
    do {
        printf("\tID: %d\n", temp->device_id);
        printf("\tDevice name: %s\n", temp->device_name);
        printf("\tDevice pin: %d\n", temp->pin);
        printf("\tGathering interval: %ds\n", temp->gathering_interval_sec);
        printf("\n");
        temp = temp->next;
    } while (temp != NULL);

    return true;
}