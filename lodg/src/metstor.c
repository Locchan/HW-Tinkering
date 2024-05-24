#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "./headers/util.h"
#include "./headers/datastrcts.h"
#include "./headers/metstor.h"
#include "./headers/globs.h"

#ifdef ENABLE_MQTT
#include "./exporters/headers/mqtt.h"
#endif

struct monitoring_data_entry* monitoring_data;
pthread_mutex_t metstor_lock;

void push_mon_entry(struct monitoring_data_entry* input_data){
    pthread_mutex_lock(&metstor_lock);
    struct monitoring_data_entry* temp_mon_ptr = monitoring_data;

    if (monitoring_data == NULL){
        monitoring_data = input_data;
        monitoring_data->head = input_data;
        monitoring_data->tail = input_data;
    } else {
        while(temp_mon_ptr != NULL){
            if(temp_mon_ptr->device_id == input_data->device_id){
                temp_mon_ptr->time = input_data->time;
                temp_mon_ptr->value = input_data->value;
                break;
            } else {
                temp_mon_ptr = temp_mon_ptr->next;
            }
        }
    }

    T_printdbg("GT: Monitoring data array length: %d\n", get_metric_arr_len());
    free(input_data);
    pthread_mutex_unlock(&metstor_lock);
}

uint32_t offload_metrics(){
    struct timespec tstart={0,0}, tend={0,0};
    long double iteration_time = 0;
    clock_gettime(CLOCK_MONOTONIC, &tstart);

    // Lock gathering thread from adding entries while we're reading all of them
    pthread_mutex_lock(&metstor_lock);

    if(monitoring_data->head == NULL){
        return 0;
    }

    int result = 0;

    if(strcmp(exporter, "FILE") == 0){
        result = offload_file(monitoring_data->head);
    }

#ifdef ENABLE_MQTT
    if(strcmp(exporter, "MQTT") == 0){
        result = offload_mqtt(monitoring_data->head);
    }
#endif
    pthread_mutex_unlock(&metstor_lock);

    if(result == -1){
        T_printf("MT: Failed to offload data!\n");
        return 0;
    }


    clock_gettime(CLOCK_MONOTONIC, &tend);
    iteration_time = ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec);
    T_printdbg("MT: Metrics offload took %Lfs\n", iteration_time);
    
    return result;

}

uint32_t get_metric_arr_len(){
    struct monitoring_data_entry* currel;
    currel = monitoring_data;
    uint32_t len = 0;
    while(currel != NULL){
        len+=1;
        currel = currel->next;  
    }
    return len;
}

int offload_file(struct monitoring_data_entry* metrics_to_offload){
    struct monitoring_data_entry* obj_to_free;
    
    int32_t iterator = 0;
    FILE *offload_fp;
    offload_fp = fopen(exporter_config, "a");

    do {
        iterator++;
        fprintf(offload_fp, "%d:%s:%s:%ld:%d\n",
            metrics_to_offload->device_id,
            metrics_to_offload->device_type,
            metrics_to_offload->device_name,
            metrics_to_offload->time,
            metrics_to_offload->value
        );
        obj_to_free = metrics_to_offload;
        metrics_to_offload = metrics_to_offload->next;
        free(obj_to_free);
    } while (metrics_to_offload != NULL);

    fclose(offload_fp);
    return iterator;
}