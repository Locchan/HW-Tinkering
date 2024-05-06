#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../headers/meteo.h"
#include "../headers/config.h"

#define MAX_LEN 128
#define LINES_TO_IGNORE 15

const char delimiter[2] = ":";

struct config* get_config(char* filepath){
    struct config* current_element = malloc(sizeof(struct config));
    struct config* new_element;
    char* key;
    char* value;
    *current_element = (struct config) {.id = NULL, .key = NULL, .value = NULL, .head = NULL, .next = NULL};
    FILE* fp;
    fp = fopen(filepath, "r");

    if (fp == NULL) {
      printf("\nTried path: %s\n", filepath);
      perror("\nCould not read config file: ");
      exit(EXIT_FAILURE);
    }

    char line_buf[MAX_LEN];
    uint8_t iterator = 0;
    bool start_found = 0;
    while (fgets(line_buf, MAX_LEN, fp))
    {
        line_buf[strcspn(line_buf, "\r\n")] = 0;

        // Ignore first 5 lines
        if(!start_found && iterator == LINES_TO_IGNORE){
            start_found = 1;
            iterator = 0;
        } else if(!start_found) {
            iterator++;
            continue;
        }

        // Only the first element is NULL-initialized
        if(current_element->head == NULL){
            current_element->head = current_element;
        } else {
            new_element = malloc(sizeof(struct config));
            *new_element = (struct config) {.id = NULL, .key = NULL, .value = NULL, .head = NULL, .next = NULL};
            current_element->next = new_element;
            new_element->head = current_element->head;
            current_element = new_element;
        }

        key = strtok(line_buf, delimiter);
        value = strtok(NULL, delimiter);
        if (key == NULL || value == NULL){
            printf("\nMalformed config.\n");
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
    const char value_delimiter[2] = " ";
    char* pin, interval;
    struct monitoring_point* current_monitoring_point = malloc(sizeof(struct monitoring_point));
    *current_monitoring_point = (struct monitoring_point) 
        { 
         .device_id=NULL, .device_name=NULL, .pin=NULL,
         .gathering_interval_sec = NULL, .head=NULL, .next=NULL
        };
    uint8_t iterator = 0;
    do {
        strncpy(current_monitoring_point->device_name, configuration->key, 16);

        strip_trailing_spaces_and_newlines(configuration->value);

        pin = strtok(configuration->value, value_delimiter);
        interval = strtok(NULL, value_delimiter);

        current_monitoring_point->device_id = iterator;

        if (pin == NULL || interval == NULL){
            printf("\nMalformed config.\n");
            printf("Malformed entry: %s\n", configuration->key);
            exit(EXIT_FAILURE);
        }

        current_monitoring_point->pin = atoi(pin);
        current_monitoring_point->gathering_interval_sec = atoi(interval);

        iterator++;
    } while(configuration != NULL);
}
