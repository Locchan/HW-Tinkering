#include <string.h>
#include <stdlib.h>
#include <mosquitto.h>

#include "../headers/datastrcts.h"
#include "../headers/mqtt.h"
#include "../headers/util.h"

const char mqtt_cfg_delimiter[1] = ":";
struct mosquitto * mosquitto_obj;

struct mqtt_config* popularize_mqtt_config(char* exporter_config){
    struct mqtt_config* config = malloc(sizeof(struct mqtt_config));
    config->address = strtok(exporter_config, mqtt_cfg_delimiter);;
    config->port = strtok(NULL, mqtt_cfg_delimiter);
    config->topic = strtok(NULL, mqtt_cfg_delimiter);
    config->username = strtok(NULL, mqtt_cfg_delimiter);
    config->password = strtok(NULL, mqtt_cfg_delimiter);
    return config;
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

char* initialize_mqtt(char* exporter_config){
    char* result = malloc(256 * sizeof(char*));
    struct mqtt_config* mqtt_cfg = popularize_mqtt_config(exporter_config);
    T_printf("Initializing MQTT: Server: %s, Port: %s, Topic: %s, Username: %s.\n",
     mqtt_cfg->address, mqtt_cfg->port, mqtt_cfg->topic, mqtt_cfg->username);

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
    T_printf("\tpublish: \"%s\";\n", mosquitto_strerror(cfg_result));
    char* config = "{\"name\":null,\"device_class\":\"motion\",\"state_topic\":\"homeassistant/binary_sensor/garden/state\",\"unique_id\":\"motion01ad\",\"device\":{\"identifiers\":[\"01ad\"],\"name\":\"Garden\"}}";
    cfg_result = mosquitto_publish(mosquitto_obj, 0, "homeassistant/binary_sensor/garden/config", strlen(config) + 1, config, 2, true);
    if(cfg_result != MOSQ_ERR_SUCCESS){
        sprintf(result, "Failed to connect to MQTT broker (mosquitto_publish). Error code: %s.", mosquitto_strerror(cfg_result));
        return result;
    }
    T_printf("Connected to MQTT broker!\n");
    return NULL;
}

int mqtt_offload_metrics(){

}