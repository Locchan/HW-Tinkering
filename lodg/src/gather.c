#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "./headers/datastrcts.h"
#include "./headers/lodg.h"
#include "./headers/config.h"
#include "./headers/metstor.h"
#include "./headers/gather.h"
#include "./headers/util.h"
#include "./gatherers/headers/gath_unix.h"

struct monitoring_data_entry* gather_metric(struct monitoring_point* point){
    time_t gathering_time = 1;
    struct monitoring_data_entry* new_data_entry = malloc(sizeof(struct monitoring_data_entry));
    *new_data_entry = (struct monitoring_data_entry) {0};
    new_data_entry->device = point;
    new_data_entry->value = get_metric_value(point);

    T_printdbg("GT: %s:%s -> %f\n", new_data_entry->device->device_name, new_data_entry->device->device_type, new_data_entry->value);

    time(&gathering_time);
    new_data_entry->time = gathering_time;
    point->last_gathered = gathering_time;
    
    return new_data_entry;
}

float get_metric_value(struct monitoring_point* point){
    #ifdef ENABLE_UNIX_GATHERERS
    if(strcmp(point->device_type, "UNXCPU") == 0){
        return get_la1();
    } else if ((strcmp(point->device_type, "UNXMAV") == 0)){
        return get_memavail();
    }
    #endif

    T_printf("Unknown device type: %s!\n", point->device_type);
    return 0;

}