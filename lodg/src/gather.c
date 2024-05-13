#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "../headers/datastrcts.h"
#include "../headers/lodg.h"
#include "../headers/config.h"
#include "../headers/metstor.h"
#include "../headers/gather.h"
#include "../headers/util.h"
#include "../headers/gath_unix.h"

struct monitoring_data_entry* gather_metric(struct monitoring_point* point){
    time_t gathering_time = 1;
    struct monitoring_data_entry* new_data_entry = malloc(sizeof(struct monitoring_data_entry));
    *new_data_entry = (struct monitoring_data_entry) {0};
    new_data_entry->device_id = point->device_id;
    new_data_entry->device_name = point->device_name;
    new_data_entry->device_type = point->device_type;
    new_data_entry->value = get_metric_value(point);

    T_printdbg("GT: %s:%s -> %d\n", new_data_entry->device_name, new_data_entry->device_type, new_data_entry->value);

    time(&gathering_time);
    new_data_entry->time = gathering_time;
    point->last_gathered = gathering_time;
    
    return new_data_entry;
}

int get_metric_value(struct monitoring_point* point){
    #ifdef ENABLE_UNIX_GATHERERS
    if(strcmp(point->device_type, "UNXCPU") == 0){
        return get_la1();
    }
    #endif

    T_printf("Unknown device type: %s!\n", point->device_type);
    return 0;

}