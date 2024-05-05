#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "headers/config.h"

struct monitoring_points {
    uint8_t device_id;
    uint8_t pin;
    uint16_t gathering_interval_sec;
};

struct monitoring_data_entry {
    uint8_t device_id;
    time_t time;
    int value;
};

int main(){
    struct config* configuration = get_config("/home/locchan/dev/HW-Tinkering/meteo/cfg/config.cfg");
    struct config* current_item = configuration;
    do {
        printf("%s\n", current_item->key);
        current_item = current_item->next;
    } while(current_item != NULL);
}