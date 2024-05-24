#ifdef ENABLE_MQTT
#ifndef MQTT_H
#define MQTT_H

#include <stdbool.h>

extern struct mqtt_config* mqtt_cfg;
char* initialize_mqtt(char* exporter_config);
struct mqtt_config* popularize_mqtt_config(char* exporter_config);
bool validate_mqtt_config(struct mqtt_config* config);
int offload_mqtt(struct monitoring_data_entry* data_to_offload);

#endif
#endif