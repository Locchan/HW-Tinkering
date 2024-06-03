typedef int stop_throwing_warnings_gcc;
#ifdef ENABLE_MQTT

#include <string.h>
#include <stdlib.h>
#include <mosquitto.h>
#include <unistd.h>

#include "../headers/datastrcts.h"
#include "./headers/mqtt.h"
#include "../headers/util.h"

#define MQTT_LOOP_PERIOD_SEC 5

const char mqtt_cfg_delimiter[1] = ":";
struct mosquitto* mosquitto_obj;
struct mqtt_config* mqtt_cfg;

void popularize_mqtt_config(char* exporter_config){
    mqtt_cfg = malloc(sizeof(struct mqtt_config));
    mqtt_cfg->address = strtok(exporter_config, mqtt_cfg_delimiter);;
    mqtt_cfg->port = strtok(NULL, mqtt_cfg_delimiter);
    mqtt_cfg->username = strtok(NULL, mqtt_cfg_delimiter);
    mqtt_cfg->password = strtok(NULL, mqtt_cfg_delimiter);
    mqtt_cfg->mqtt_unique_device_id = strtok(NULL, mqtt_cfg_delimiter);
    mqtt_cfg->mqtt_device_name = strtok(NULL, mqtt_cfg_delimiter);
}

bool validate_mqtt_config(struct mqtt_config* config){
    bool config_ok = true;
    if(!validate_address(config->address, config->port)){
        config_ok = false;
    }
    if(config->username == NULL || strlen(config->username) == 0 ||
       config->password == NULL || strlen(config->password) == 0){
        config_ok = false;
    }
    return config_ok;
}

char* initialize_mqtt(char* exporter_config, struct monitoring_point* monitoring_points){
    popularize_mqtt_config(exporter_config);

    char* result = malloc(256 * sizeof(char*));
    T_printf("Initializing MQTT: Server: %s, Port: %s, Username: %s.\n",
     mqtt_cfg->address, mqtt_cfg->port, mqtt_cfg->username);

    if(!validate_mqtt_config(mqtt_cfg)){
        result = "MQTT config is invalid. Check address/port and username/password.";
        return result;
    }

    int cfg_result = mosquitto_lib_init();
    if(cfg_result != MOSQ_ERR_SUCCESS){
        sprintf(result, "MQTT library failed to initialize. Error code: %s.", mosquitto_strerror(cfg_result));
        return result;
    }

    int major, minor, revision;
    mosquitto_lib_version(&major, &minor, &revision);
    T_printf("libmosquitto %d.%d.%d initialized.\n", major, minor, revision);

    T_printf("Connecting to MQTT broker...\n");
    mosquitto_obj = mosquitto_new("mqtt-client", true, NULL);
    mosquitto_username_pw_set(mosquitto_obj, mqtt_cfg->username, mqtt_cfg->password);

    uint16_t port_int;
    port_int = (uint16_t) atoi(mqtt_cfg->port);
    cfg_result = mosquitto_connect(mosquitto_obj, mqtt_cfg->address, port_int, 60);
    T_printf("\tconnect: \"%s\";\n", mosquitto_strerror(cfg_result));
    if(cfg_result != MOSQ_ERR_SUCCESS){
        sprintf(result, "Failed to connect to MQTT broker (mosquitto_connect). Error code: %s.", mosquitto_strerror(cfg_result));
        return result;
    }
    
    cfg_result = mosquitto_loop(mosquitto_obj, 60, 1);
    T_printf("\tloop: \"%s\";\n", mosquitto_strerror(cfg_result));
    if(cfg_result != MOSQ_ERR_SUCCESS){
        sprintf(result, "Failed to connect to MQTT broker (mosquitto_loop). Error code: %s.", mosquitto_strerror(cfg_result));
        return result;
    }

    T_printf("Connected to MQTT broker!\n");
    return register_metrics(monitoring_points, result);
}

char* register_metrics(struct monitoring_point* monitoring_points, char* result){
    int cfg_result;
    struct monitoring_point* temp_ptr = monitoring_points->head;
    char* payload = malloc(512 * sizeof(char));
    char* config_topic = malloc(256 * sizeof(char));
    char* state_topic = malloc(256 * sizeof(char));
    while(temp_ptr != NULL) {
        sprintf(config_topic, "homeassistant/sensor/%s/%s_%d/config", mqtt_cfg->mqtt_unique_device_id, temp_ptr->device_name, temp_ptr->device_id);
        sprintf(state_topic, "homeassistant/binary_sensor/%s/state", mqtt_cfg->mqtt_unique_device_id);
        sprintf(payload, "{ \"name\": \"%s:%s\", \"unique_id\": \"%s_%s_%s_%d\", \"state_topic\": \"%s\", \"unit_of_measurement\": \"%s\", \"device\": {\"identifiers\": [\"%s\"], \"name\": \"%s\", \"manufacturer\": \"Locchan\", \"model\": \"Prototype\"}, \"value_template\": \"{{ value_json.%s_%d }}\" }",
            temp_ptr->device_name, temp_ptr->device_type,
            mqtt_cfg->mqtt_unique_device_id, temp_ptr->device_type, temp_ptr->device_name, temp_ptr->device_id,
            state_topic,
            temp_ptr->device_measurement_unit,
            mqtt_cfg->mqtt_unique_device_id,
            mqtt_cfg->mqtt_device_name,
            temp_ptr->device_name, temp_ptr->device_id
        );
        T_printdbg("Initializing device:\n\t  Topic: %s\n\tPayload: %s\n", config_topic, payload);
        cfg_result = mosquitto_publish(mosquitto_obj, NULL, config_topic, strlen(payload) * sizeof(char), payload, 0, true);
        if(cfg_result != MOSQ_ERR_SUCCESS){
            sprintf(result, "Failed to register a device. Error code: %s.", mosquitto_strerror(cfg_result));
            free(state_topic);
            free(config_topic);
            free(payload);
            return result;
        }
        temp_ptr = temp_ptr->next;
    }
    free(state_topic);
    free(config_topic);
    free(payload);
    return NULL;
}

void* mqtt_loop(){
    T_printf("MQT: MQTT Thread started.\n");
    int mqtt_result;
    while(true){
        mqtt_result = mosquitto_loop(mosquitto_obj, 60, 1);
        if(mqtt_result != MOSQ_ERR_SUCCESS){
            T_printf("MQT: MQTT loop failed. Error code: %s.\n", mosquitto_strerror(mqtt_result));
        }
        sleep(MQTT_LOOP_PERIOD_SEC);
    }
}

char* mqtt_generate_data_json(struct monitoring_data_entry* data_to_publish){
    char* result_json = malloc(2048 * sizeof(char));
    char* item = malloc(32 * sizeof(char));
    memset(result_json, 0, 2048 * sizeof(char));
    result_json[2047] = '\0';
    strcat(result_json, "{");
    struct monitoring_data_entry* tmp_data_ptr;
    tmp_data_ptr = data_to_publish->head;
    while (true) {
        sprintf(item, "\"%s_%d\": %f", tmp_data_ptr->device->device_name, tmp_data_ptr->device->device_id, tmp_data_ptr->value);
        strcat(result_json, item);
        tmp_data_ptr = tmp_data_ptr->next;
        if (tmp_data_ptr == NULL){
            break;
        } else {
            strcat(result_json, ", ");
        }
    };
    strcat(result_json, "}\0");
    free(item);
    return result_json;
}

int mqtt_publish_state(char* json_formatted_data){
    int send_result;
    char* data_topic = malloc(256 * sizeof(char));
    sprintf(data_topic, "homeassistant/binary_sensor/%s/state", mqtt_cfg->mqtt_unique_device_id);
    T_printdbg("OT: Publishing data: %s\n", json_formatted_data);
    send_result = mosquitto_publish(mosquitto_obj, NULL, data_topic, strlen(json_formatted_data) * sizeof(char), json_formatted_data, 0, false);
    if(send_result != MOSQ_ERR_SUCCESS){
        T_printf("Failed to publish data. Error code: %s.", mosquitto_strerror(send_result));
        return 0;
    }
    free(data_topic);
    free(json_formatted_data);
    return -1;
}

#endif