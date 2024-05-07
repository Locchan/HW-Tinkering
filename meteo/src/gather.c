#include <stdlib.h>
#include <time.h>

#include "../headers/datastrcts.h"
#include "../headers/meteo.h"
#include "../headers/config.h"
#include "../headers/metstor.h"
#include "../headers/gather.h"

struct monitoring_data_entry* gather_metric(struct monitoring_point* point){
    time_t gathering_time = 1;
    struct monitoring_data_entry* new_data_entry = malloc(sizeof(struct monitoring_data_entry));
    *new_data_entry = (struct monitoring_data_entry) {0};
    new_data_entry->device_id = point->device_id;

    new_data_entry->value = get_metric_value(point);

    time(&gathering_time);
    new_data_entry->time = gathering_time;
    point->last_gathered = gathering_time;
    
    return new_data_entry;
}

int get_metric_value(struct monitoring_point* point){
    return point->pin + 547;
}