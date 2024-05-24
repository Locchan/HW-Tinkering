#ifndef LODG_H
#define LODG_H

#include <stdbool.h>

int main_wrapped(int argc, char** argv);
bool initialize(char* cfg_filepath);
char* initialize_exporter();
void start_threads();
void main_loop();
void printver();
void* gathering_loop();
void gathering_loop_iteration();
void* offload_loop();

#endif
