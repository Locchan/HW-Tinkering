#include <sys/sysinfo.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/util.h"

uint64_t get_free_memory(){
    uint64_t pages = sysconf(_SC_AVPHYS_PAGES);
    uint32_t page_size = sysconf(_SC_PAGESIZE);
    return (uint64_t) (pages * page_size);
}

void print_help(){
    printf("USAGE:\n\t-h\t\t\t\tShow help message\n\t-c CONFIG_PATH\t\t\tSet configuration path.\n");
}

void strip_trailing_spaces_and_newlines(char* str){
    uint8_t length = strlen(str);
    char* temp_str = malloc(length * sizeof(char));
    strcpy(temp_str, str);

    uint8_t first_non_space_char;
    uint8_t last_non_space_char;

    uint8_t i;

    for (i = 0; i < length; i++){
        if(str[i] != ' ' && str[i] != '\n'){
            first_non_space_char = i;
            break;
        }
    }

    for (i = first_non_space_char; i < length; i++){
        if(str[i] != ' ' && str[i] != '\n'){
            last_non_space_char = i;
        }
    }

    strncpy(str, temp_str + first_non_space_char, last_non_space_char - first_non_space_char);
    free(temp_str);
}