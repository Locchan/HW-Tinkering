#ifdef ENABLE_UNIX_GATHERERS
#include <stdint.h>
#include <stdio.h>

// Unix gatherers are obviously unix-specific
//  debug-build will put test data in build directory
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