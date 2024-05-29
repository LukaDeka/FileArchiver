#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define BOLD  "\033[1m"
#define RESET "\033[0m"
#define RED   "\033[31m"
#define GREEN "\033[32m"

typedef struct {
    uint16_t output   : 1;
    uint16_t output_i : 4;
    uint16_t input_i  : 4;
    uint16_t info     : 1;
    uint16_t encode   : 1;
    uint16_t decode   : 1;
} Flags;

size_t get_filesize(char* filename);

void assert_file(char* filename);

bool has_huff_extension(char* arg);

Flags* check_flags(int argc, char** argv);


#endif
