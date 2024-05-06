#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "../headers/meteo.h"
#include "../headers/config.h"
#include "../headers/util.h"

int main(int argc, char** argv){

    char* config_filepath;
    uint64_t freemem = get_free_memory();

    // Parse cmdline arguments
    for (int i = 0; i < argc; i++){
        if(strcmp(argv[i], "-h") == 0){
            print_help();
            exit(EXIT_SUCCESS);
        }
        if(strcmp(argv[i], "-c") == 0){
            config_filepath = argv[i+1];
            i++;
        }
    }

    if(config_filepath == NULL){
        print_help();
        exit(EXIT_FAILURE);
    }

    printf("Meteo data gatherer. Starting...\n");
    printf("Free memory: %ldK\n", freemem / 1024);
    bool config_result = initialize(config_filepath);

    if (config_result == false){
        printf("Could not configure.\n");
        exit(EXIT_FAILURE);
    }

    
}

bool initialize(char* cfg_filepath){
    printf("Config file path is: %s\n", cfg_filepath);
    printf("Loading config...");
    struct config* configuration = get_config(cfg_filepath);

    if (configuration == NULL){
        printf("Empty configuration!\n");
        return false;
    }

    struct monitoring_point* monitoring_points = configure_monitoring_points(configuration);
    printf(" Done.\n");
    
    return true;
}