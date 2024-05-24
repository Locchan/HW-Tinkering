#ifndef GLOBS_H
#define GLOBS_H

#include <pthread.h>

extern char* version;
extern char* exporter;
extern bool debug;
extern char* exporter_config;
extern pthread_mutex_t metstor_lock;
extern pthread_mutex_t print_lock;
extern char* debug_meminfo_location;

#endif