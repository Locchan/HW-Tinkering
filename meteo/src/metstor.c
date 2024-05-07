#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

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
    pthread_mutex_unlock(&metstor_lock);
}

uint32_t offload_metrics(){
    int32_t iterator = 0;
    // Lock gathering thread from adding entries while we're stealing all of them
    pthread_mutex_lock(&metstor_lock);
    struct monitoring_data_entry* metrics_to_offload = monitoring_data->head;
    struct monitoring_data_entry* obj_to_free;
    // Purging (logically) all monitoring data.
    monitoring_data = NULL;
    pthread_mutex_unlock(&metstor_lock);

    if(metrics_to_offload == NULL){
        return 0;
    }

    FILE *offload_fp;
    offload_fp = fopen(offload_location, "a");

    do {
        iterator++;
        fprintf(offload_fp, "%d:%ld:%d\n",
            metrics_to_offload->device_id,
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