#ifndef DATASTRCTS_H
#define DATASTRCTS_H

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
    char device_measurement_unit[16];
    char cfg[33];
    uint32_t gathering_interval_sec; // 10 chars max
    time_t last_gathered;
    struct monitoring_point* head;
    struct monitoring_point* next;
};

struct monitoring_data_entry {
    struct monitoring_point* device;
    time_t time;
    float value;
    struct monitoring_data_entry* head;
    struct monitoring_data_entry* tail;
    struct monitoring_data_entry* next;
};

struct mqtt_config {
    char* address;
    char* port;
    char* topic;
    char* username;
    char* password;
};

#endif