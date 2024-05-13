#ifndef MQTT_H
#define MQTT_H

#include <stdbool.h>

extern struct mqtt_config* mqtt_cfg;
char* initialize_mqtt(char* exporter_config);
struct mqtt_config* popularize_mqtt_config(char* exporter_config);
bool validate_mqtt_config(struct mqtt_config* config);

#endif