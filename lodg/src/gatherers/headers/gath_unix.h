#ifndef GATH_UNIX_H
#define GATH_UNIX_H

#ifdef ENABLE_UNIX_GATHERERS

#include <stdint.h>

float get_cpu_load();
float get_memavail();

#endif

#endif