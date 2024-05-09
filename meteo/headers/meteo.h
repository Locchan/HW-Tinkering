#include <stdbool.h>

int main_wrapped(int argc, char** argv);
bool initialize(char* cfg_filepath);
void start_threads();
void main_loop();
void* gathering_loop();
void gathering_loop_iteration();
void* offload_loop();