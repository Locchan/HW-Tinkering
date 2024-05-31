#define _GNU_SOURCE
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
    bool data_added = false;
    pthread_mutex_lock(&metstor_lock);
    struct monitoring_data_entry* temp_mon_ptr = monitoring_data;

    // Initialize monitoring data array
    if (temp_mon_ptr == NULL){
        monitoring_data = input_data;
        monitoring_data->head = monitoring_data;
        monitoring_data->tail = monitoring_data;
        data_added = true;
    } else {
        // If array is initialized, add new data to monitoring array
        while(temp_mon_ptr != NULL){
            if(temp_mon_ptr->device->device_id == input_data->device->device_id){
                temp_mon_ptr->time = input_data->time;
                temp_mon_ptr->value = input_data->value;
                free(input_data);
                data_added = true;
                break;
            } else {
                temp_mon_ptr = temp_mon_ptr->next;
            }
        }
    }

    // If we didn't add the data, we have to create corresponding entry in monitoring array
    if (!data_added){
        monitoring_data->tail->next = input_data;
        monitoring_data->tail = input_data;
        data_added = true;
    } 


    T_printdbg("GT: Monitoring data array length: %d\n", get_metric_arr_len(true));
    pthread_mutex_unlock(&metstor_lock);
}

uint32_t offload_metrics(){
    struct timespec tstart={0,0}, tend={0,0};
    long double iteration_time = 0;
    clock_gettime(CLOCK_MONOTONIC, &tstart);

    // Lock gathering thread from adding entries while we're reading all of them
    pthread_mutex_lock(&metstor_lock);

    if(monitoring_data->head == NULL){
        pthread_mutex_unlock(&metstor_lock);
        return 0;
    }

    int result = 0;

    if(strcmp(exporter, "FILE") == 0){
        result = offload_file();
        pthread_mutex_unlock(&metstor_lock);
    }

#ifdef ENABLE_MQTT
    if(strcmp(exporter, "MQTT") == 0){
        // If we successfully export metrics, we export all of them. Otherwise we export none
        result = get_metric_arr_len(true);
        char* data_to_send = mqtt_generate_data_json(monitoring_data->head);
        pthread_mutex_unlock(&metstor_lock);
        if (mqtt_publish_state(data_to_send) == -1){
            return result;
        } else {
            return 0;
        }
    }
#endif


    if(result == -1){
        T_printf("MT: Failed to offload data!\n");
        return 0;
    }


    clock_gettime(CLOCK_MONOTONIC, &tend);
    iteration_time = ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec);
    T_printdbg("MT: Metrics offload took %Lfs\n", iteration_time);
    
    return result;

}

uint32_t get_metric_arr_len(bool ignore_mutex){
    if(!ignore_mutex){
        pthread_mutex_lock(&metstor_lock);
    }
    struct monitoring_data_entry* currel;
    currel = monitoring_data->head;
    uint32_t len = 0;
    while(currel != NULL){
        len+=1;
        currel = currel->next;  
    }
    if(!ignore_mutex){
        pthread_mutex_unlock(&metstor_lock);
    }
    return len;
}

int offload_file(){
    struct monitoring_data_entry* tmp_mon_ptr;
    tmp_mon_ptr = monitoring_data->head;
    int32_t iterator = 0;
    FILE *offload_fp;
    offload_fp = fopen(exporter_config, "a");

    do {
        iterator++;
        fprintf(offload_fp, "%d:%s:%s:%ld:%f:%s\n",
            tmp_mon_ptr->device->device_id,
            tmp_mon_ptr->device->device_type,
            tmp_mon_ptr->device->device_name,
            tmp_mon_ptr->time,
            tmp_mon_ptr->value,
            tmp_mon_ptr->device->device_measurement_unit
        );
        tmp_mon_ptr = tmp_mon_ptr->next;
    } while (tmp_mon_ptr != NULL);

    fclose(offload_fp);
    return iterator;
}