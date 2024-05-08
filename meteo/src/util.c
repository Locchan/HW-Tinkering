#include <sys/sysinfo.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <malloc.h>

#include "../headers/datastrcts.h"
#include "../headers/util.h"
#include "../headers/globs.h"

uint64_t get_free_memory(){
    uint64_t pages = sysconf(_SC_AVPHYS_PAGES);
    uint32_t page_size = sysconf(_SC_PAGESIZE);
    return (uint64_t) (pages * page_size);
}

void print_help(){
    T_printf("USAGE:\n\t-h\n\t\t\tShow help message\n");
    T_printf("\t-c CONFIG_PATH\n\t\t\tSet configuration path.\n");
    T_printf("\t-o OFFLOAD\n\t\t\tPath to periodically offload metrics to.\n");
}

void strip(char* str){
    uint8_t length = strlen(str);
    char* temp_str = malloc((length + 1) * sizeof(char));
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
    substring(temp_str, first_non_space_char, last_non_space_char - first_non_space_char + 1, str);
    free(temp_str);
}

// Creates a substring provided input, offset, length
//  (how much chars to substring starting with offset) and dest
// len of -1 means "Until the end of the string"
void substring(const char* input, uint8_t offset, int16_t len, char* dest){
  uint8_t input_len = strlen(input);

  if(len == 0 || input_len == 0){
    strcpy(dest, input);
    return;
  }

  if (offset + len > input_len){
    T_printf("Substring error.\n");
    T_printf("Params: input \"%s\"; offset \"%d\"; length \"%d\"\n", input, offset, len);
    return;
  }

  if (len != -1){
    memcpy(dest, input + offset * sizeof(char), len);
    dest[len] = '\0';
  } else {
    uint8_t new_len = input_len - offset;
    memcpy(dest, input + offset, new_len);
    dest[new_len] = '\0';
  }
}

// Finds a first matching character inside a string and returns its position
// Returns -1 is the char is not found
int16_t findchr(char* str, const char* token){
    uint8_t string_len = strlen(str);
    for (int16_t i = 0; i < string_len; i++){
        if (str[i] == *token){
            return i;
        }
    }
    return -1;
}

void free_unused_heap(){
    malloc_trim(0);
}

void print_memory_stats(){
    if (debug) {
        struct mallinfo2 memstat = mallinfo2();
        T_printdbg("\tTotal non-mmapped bytes (arena):       %zu\n", memstat.arena);
        T_printdbg("\t# of free chunks (ordblks):            %zu\n", memstat.ordblks);
        T_printdbg("\t# of free fastbin blocks (smblks):     %zu\n", memstat.smblks);
        T_printdbg("\t# of mapped regions (hblks):           %zu\n", memstat.hblks);
        T_printdbg("\tBytes in mapped regions (hblkhd):      %zu\n", memstat.hblkhd);
        T_printdbg("\tMax. total allocated space (usmblks):  %zu\n", memstat.usmblks);
        T_printdbg("\tFree bytes held in fastbins (fsmblks): %zu\n", memstat.fsmblks);
        T_printdbg("\tTotal allocated space (uordblks):      %zu\n", memstat.uordblks);
        T_printdbg("\tTotal free space (fordblks):           %zu\n", memstat.fordblks);
        T_printdbg("\tTopmost releasable block (keepcost):   %zu\n", memstat.keepcost);
    }
}

void T_printdbg(const char *format, ...){
    pthread_mutex_lock(&print_lock);
    va_list ap;
    if (debug) {
        va_start(ap, format);
        vprintf(format, ap);
        va_end(ap);
    }
    pthread_mutex_unlock(&print_lock);
}

void T_printf(const char *format, ...){
    pthread_mutex_lock(&print_lock);
    va_list ap;
    va_start(ap, format);
    vprintf(format, ap);
    va_end(ap);
    pthread_mutex_unlock(&print_lock);
}