#include <stdint.h>
#include <malloc.h>

uint64_t get_free_memory();
void print_help();
void strip (char* str);
void substring(const char* input, uint8_t offset, int16_t len, char* dest);
int16_t findchr(char* str, const char* token);
void free_unused_heap();
void print_memory_stats();
void export_memory_stats(struct mallinfo2 memstat);
void T_printdbg(const char *format, ...);
void T_printf(const char *format, ...);