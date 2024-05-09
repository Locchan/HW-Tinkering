#include <stdint.h>
#include <stdbool.h>
#include <time.h>

struct config {
    uint8_t id;
    char* key;
    char* value;
    struct config* head;
    struct config* next;
};

struct monitoring_point {
    uint8_t device_id;
    char device_name[11];
    char device_type[7];
    uint8_t pin;
    uint32_t gathering_interval_sec;
    time_t last_gathered;
    struct monitoring_point* head;
    struct monitoring_point* next;
};

struct monitoring_data_entry {
    uint8_t device_id;
    time_t time;
    int value;
    struct monitoring_data_entry* head;
    struct monitoring_data_entry* tail;
    struct monitoring_data_entry* next;
};