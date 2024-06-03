#ifdef ENABLE_UNIX_GATHERERS
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/util.h"

// Unix gatherers are obviously unix-specific
//  debug build will put test data in build directory
//  so that the gatherers can be debugged on non-unix OSs

#ifdef __unix__
char* loadavg_path = "/proc/loadavg";
char* meminfo_path = "/proc/meminfo";
#elif defined(_WIN32) || defined(WIN32)
char* loadavg_path = "./loadavg";
char* meminfo_path = "./meminfo";
#endif

float get_la1(){
    // cpu  1417 0 2678 73606 0 0 145 0 0 0
    FILE *fp;
    float la1;
    fp = fopen(loadavg_path, "r");
    if(fp == NULL){
        return 255;
    }else{
        fscanf(fp, "%f", &la1);
        fclose(fp);
    }
    return la1 * 100;
}

float get_memavail(){
    // MemAvailable:      15364 kB
    FILE *fp;
    int memavail = 0;
    char* linetofind = "MemAvailable";
    char* line_buf = malloc(256 * sizeof(char));
    fp = fopen(meminfo_path, "r");
    while (fgets(line_buf, 255, fp)){
        if(strncmp(linetofind, line_buf, strlen(linetofind)) == 0){
            if(sscanf(line_buf, "%*[^0123456789]%d", &memavail)){
                break;
            } else {
                T_printf("Error while parsing meminfo. Pattern does not match.\n");
                return -1;
            }
        }
    }
    free(line_buf);
    return (float) memavail;
}

#endif