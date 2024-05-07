bool initialize(char* cfg_filepath);
void start_threads();
void* gathering_loop();
void gathering_loop_iteration();
void* offload_loop();