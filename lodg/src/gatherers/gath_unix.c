#ifdef ENABLE_UNIX_GATHERERS
#include <stdint.h>
#include <stdio.h>

float get_la1(){
    // cpu  1417 0 2678 73606 0 0 145 0 0 0
    FILE *fp;
    float la1;
    fp = fopen("/proc/loadavg", "r");
    if(fp == NULL){
        return 255;
    }else{
        fscanf(fp, "%f", &la1);
        fclose(fp);
    }
    return la1 * 100;
}
#endif