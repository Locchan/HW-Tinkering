#ifdef ENABLE_MQTT
#ifndef MQTT_H
#define MQTT_H

#include <stdbool.h>
#include <mosquitto.h>

extern struct mqtt_config* mqtt_cfg;
char* initialize_mqtt(char* exporter_config, struct monitoring_point* monitoring_points);
void popularize_mqtt_config(char* exporter_config);
bool validate_mqtt_config(struct mqtt_config* config);
int mqtt_publish_state(char* json_formatted_data);
char* register_metrics(struct monitoring_point* monitoring_points, char* result);
char* mqtt_generate_data_json(struct monitoring_data_entry* data_to_publish);
void* mqtt_loop();
#endif
#endif