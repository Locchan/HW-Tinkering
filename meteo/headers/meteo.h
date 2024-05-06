#include <time.h>
#include <stdint.h>
#include <stdbool.h>

struct monitoring_data_entry {
    uint8_t device_id;
    time_t time;
    int value;
    struct monitoring_data_entry* head;
    struct monitoring_data_entry* next;
};

bool initialize(char* cfg_filepath);