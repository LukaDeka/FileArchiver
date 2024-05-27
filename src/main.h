#ifndef MAIN_H
#define MAIN_H

#include <string.h>
#include <stdbool.h>

#define BOLD  "\033[1m"
#define RESET "\033[0m"
#define RED   "\033[31m"
#define GREEN "\033[32m"

int main(int argc, char* argv[]);

bool has_huff_extension(char* arg);

bool has_demo_flag(char* arg);

size_t get_filesize(char* filename);


#endif
