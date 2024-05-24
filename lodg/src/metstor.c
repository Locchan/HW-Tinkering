#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include "../headers/util.h"
#include "../headers/datastrcts.h"
#include "../headers/metstor.h"
#include "../headers/globs.h"

struct monitoring_data_entry* monitoring_data;
pthread_mutex_t metstor_lock;

void push_mon_entry(struct monitoring_data_entry* input_data){
    pthread_mutex_lock(&metstor_lock);
    struct monitoring_data_entry* prev_last;

    if (monitoring_data == NULL){
        monitoring_data = input_data;
        monitoring_data->head = input_data;
        monitoring_data->tail = input_data;
    } else {
        input_data->head = monitoring_data->head;
        prev_last = monitoring_data->tail;
        monitoring_data->tail = input_data;
        prev_last->next = input_data;
    }
    if(debug){
        T_printdbg("Monitoring data array length: %d\n", get_metric_arr_len());
    }
    pthread_mutex_unlock(&metstor_lock);
}

uint32_t offload_metrics(){
    // Lock gathering thread from adding entries while we're stealing all of them
    pthread_mutex_lock(&metstor_lock);
    struct monitoring_data_entry* metrics_to_offload = monitoring_data->head;
    // Stealing all monitoring data from gathering thread.
    monitoring_data = NULL;
    pthread_mutex_unlock(&metstor_lock);
    if(metrics_to_offload == NULL){
        return 0;
    }

    int result = 0;

    if(strcmp(exporter, "FILE") == 0){
        result = offload_file(metrics_to_offload);
    }

    if(strcmp(exporter, "MQTT") == 0){
        result = offload_mqtt(metrics_to_offload);
    }

    if(result == -1){
        T_printf("Failed to offload data!\n");
        return_data(metrics_to_offload);
        return 0;
    }

    return result;

}

// Returns data if offloading fails for some reason. Prepends data to the start
void return_data(struct monitoring_data_entry* metrics_to_return){
    pthread_mutex_lock(&metstor_lock);
    if (monitoring_data != NULL){
        struct monitoring_data_entry* oldmondata = monitoring_data->head;
        monitoring_data = NULL;
        monitoring_data = metrics_to_return->head;
        monitoring_data->tail->next = oldmondata;
        monitoring_data->tail = oldmondata->tail;
    } else {
        monitoring_data = metrics_to_return;
    }

    pthread_mutex_unlock(&metstor_lock);
}

int offload_mqtt(struct monitoring_data_entry* metrics_to_offload){
    return -1;
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