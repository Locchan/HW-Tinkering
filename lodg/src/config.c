#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "./headers/datastrcts.h"
#include "./headers/lodg.h"
#include "./headers/config.h"
#include "./headers/util.h"

#define MAX_LEN 128
#define LINES_TO_IGNORE 15

const char cfg_delimiter[1] = ":";
char err[256];

struct config* get_config(char* filepath){
    struct config* current_element = malloc(sizeof(struct config));
    struct config* new_element;
    char* key;
    char* value;
    *current_element = (struct config) {0};
    FILE* fp;
    fp = fopen(filepath, "r");

    if (fp == NULL) {
      T_printf("\nTried path: %s\n", filepath);
      perror("\nCould not read config file: ");
      exit(EXIT_FAILURE);
    }

    char line_buf[MAX_LEN];
    uint8_t iterator = 0;
    bool start_found = 0;
    while (fgets(line_buf, MAX_LEN, fp)){
        line_buf[strcspn(line_buf, "\r\n")] = 0;

        // Ignore first 5 lines
        if(!start_found && iterator == LINES_TO_IGNORE){
            start_found = 1;
            iterator = 0;
        } else if(!start_found) {
            iterator++;
            continue;
        }

        // Only the first element has no head
        if(current_element->head == NULL){
            current_element->head = current_element;
        } else {
            new_element = malloc(sizeof(struct config));
            *new_element = (struct config) {0};
            new_element->head = current_element->head;
            current_element->next = new_element;
            current_element = new_element;
        }

        key = strtok(line_buf, cfg_delimiter);
        value = strtok(NULL, cfg_delimiter);
        if (key == NULL || value == NULL){
            T_printf("\nMalformed config.\n");
            exit(EXIT_FAILURE);
        }

        current_element->id = iterator;
        current_element->key = malloc((strlen(key) + 1) * sizeof(char));
        current_element->value = malloc((strlen(value) + 1) * sizeof(char));
        strcpy(current_element->key, key);
        strcpy(current_element->value, value);

        iterator++;
    }

    fclose(fp);
    return current_element->head;
}

struct monitoring_point* configure_monitoring_points(struct config* configuration){
    const char* value_delimiter = " ";
    char* interval = malloc(10 * sizeof(char));
    struct monitoring_point* current_monitoring_point = malloc(sizeof(struct monitoring_point));
    *current_monitoring_point = (struct monitoring_point) {0};
    uint8_t iterator = 0;
    do {
        // Only the first element has no head
        if(current_monitoring_point->head == NULL){
            current_monitoring_point->head = current_monitoring_point;
        } else {
            struct monitoring_point* new_monitoring_point = malloc(sizeof(struct monitoring_point));
            *new_monitoring_point = (struct monitoring_point){0};
            new_monitoring_point->head = current_monitoring_point->head;
            current_monitoring_point->next = new_monitoring_point;
            current_monitoring_point = new_monitoring_point;
        }

        if (strlen(configuration->key) < 7){
            sprintf(err, "Malformed entry. Key is too small (min 7 characters).");
            err_malformed_config();
        }

        // Parse device type and name
        substring(configuration->key, 0, 6, current_monitoring_point->device_type);
        substring(configuration->key, 6, -1, current_monitoring_point->device_name);

        strip(configuration->value);

        char* value = malloc((strlen(configuration->value) + 1) * sizeof(char));
        strcpy(value, configuration->value);

        // find the first space in the string to get measurement unit
        int16_t first_delimiter_pos = findchr(value, value_delimiter, 0);
        
        if (first_delimiter_pos == -1){
            sprintf(err, "Malformed entry (key value): {\"%s\": \"%s\"}\n", configuration->key, configuration->value);
            err_malformed_config();
        }

        if (strlen(configuration->value) > 60){
            sprintf(err, "Malformed entry (%s): value is too long! (max 58 chars)\n", configuration->key);
            err_malformed_config();
        }

        substring(configuration->value, 0, first_delimiter_pos, current_monitoring_point->device_measurement_unit);
        
        // find the second space in the string to get config
        int16_t second_delimiter_pos = findchr(value, value_delimiter, 1);

        substring(configuration->value, first_delimiter_pos + 1, second_delimiter_pos, current_monitoring_point->cfg);
        strip(current_monitoring_point->cfg);

        substring(configuration->value, second_delimiter_pos, -1, interval);
        strip(interval);

        free(value);

        current_monitoring_point->device_id = iterator;

        if (interval == NULL){
            sprintf(err, "Malformed entry: %s\n", configuration->key);
            err_malformed_config();
        }

        current_monitoring_point->gathering_interval_sec = atoi(interval);
        if(current_monitoring_point->gathering_interval_sec < 1){
            sprintf(err, "Invalid gathering interval: %d\n", current_monitoring_point->gathering_interval_sec);
            err_malformed_config();
        }
        
        iterator++;
        configuration = configuration->next;
    } while(configuration != NULL);

    free(interval);
    return current_monitoring_point->head;
}

void err_malformed_config(){
    T_printf("\nError: Malformed config:\n");
    if (strlen(err) != 0){
        T_printf("%s\n", err);
    }
    exit(EXIT_FAILURE);
}
