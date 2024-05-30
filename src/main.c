#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "main.h"
#include "IO.h"

size_t get_filesize(char* filename) {
    FILE* fp = fopen(filename, "rb");
    fseek(fp, 0, SEEK_END);
    size_t filesize = ftell(fp);
    fclose(fp);
    return filesize;
}

void assert_file(char* filename) {
    if (access(filename, F_OK) != 0) {
        fprintf(stderr, RED "Error: Input file doesn't exist!\n" RESET);
        exit(1);
    } else if (access(filename, R_OK) != 0) {
        fprintf(stderr, RED "Error: No reading permissions for input file!\n" RESET);
        exit(1);
    }
}

bool has_huff_extension(char* arg) {
    char* extension = strrchr(arg, '.');
    if (extension == NULL) { return false; }
    return strcmp(strrchr(arg, '.'), ".huff") ? false : true;
}

Flags* check_flags(int argc, char** argv) {
    Flags* flags = malloc(sizeof(Flags));
    flags->output   = false;
    flags->output_i = 0;
    flags->input_i  = 0;
    flags->info     = false;
    flags->encode   = false;
    flags->decode   = false;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output")) {

            if (i == argc - 1) {
                fprintf(stderr, RED "Error: Missing output file name!\n" RESET);
                exit(1);
            } else if (!has_huff_extension(argv[i + 1])) {
                fprintf(stderr, RED "Error: Output file must be a .huff file!\n" RESET);
                exit(1);
            } else {
                flags->encode = true;
                flags->output = true;
                flags->output_i = ++i;
            }
        } else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            printf(BOLD "\nUsage:\n" RESET);
            printf("\tProvide a file you want to compress/decompress like so:\n");
            printf(GREEN "\t./main [input] [flags]\n" RESET);
            printf(BOLD "Optional flags:\n" RESET);
            printf("\t-o, --output: Output filename.\n");
            printf("\t-i, --info:   Show compression info.\n");
            printf("\t-h, --help:   Print this message.\n");
            printf(BOLD "Notes:\n" RESET);
            printf("\tProgram encodes/decodes based on the filename.\n");
            printf("\t-o and -i cannot be used when decompressing.\n");
            exit(0);
        } else if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--info")) {
            flags->info = true;
        } else if (has_huff_extension(argv[i])) {
            flags->decode = true;
            flags->input_i = i;
        } else if (!has_huff_extension(argv[i])) {
            flags->encode = true;
            flags->input_i = i;
        } else {
            fprintf(stderr, RED "Error: Invalid argument! Try --help\n" RESET);
            exit(1);
        }
    }

    if (flags->encode && flags->decode) {
        fprintf(stderr, RED "Parsing error: Please check the arguments again.\n" RESET);
        exit(1);
    }

    if (flags->decode && flags->info) {
        fprintf(stderr, RED "Error: --info only available for encoding!\n" RESET);
        fprintf(stderr, RED "Continuing without...\n" RESET);
        flags->info = false;
    }

    assert_file(argv[flags->input_i]);

    return flags;
}

/*
    USAGE:
        To encode:
            ./main image.bmp
            ./main text.txt -o my_archive.huff
        To decode:
            ./main file.huff
    FLAGS:
        -o, --output
            specifies the output file name
            Note: only possible when encoding
        -i, --info
            prints info about the compression ratio
            Note: only possible when encoding
        -h, --help
            prints information about usage
    NOTES:
        Program encodes/decodes based on the filename.
        The default output name is file.huff
        To decode, the file extension must be .huff
        To encode, the file extension must not be .huff
*/
int main(int argc, char* argv[]) {
    if (argc == 1) {
        fprintf(stderr, RED "Error: Please provide a filename.\n" RESET);
        return 1;
    }

    Flags* flags = check_flags(argc, argv);
    if (flags->encode) {
        char* in_filename = argv[flags->input_i];
        char* out_filename = flags->output ? argv[flags->output_i] : "file.huff";

        encode(in_filename, out_filename);

        printf(GREEN "\nSuccess!\n" RESET);
        if (flags->info) {
            size_t original_filesize = get_filesize(in_filename);
            size_t encoded_filesize  = get_filesize(out_filename);
            float  original_filesize_in_mb = (float) original_filesize / (1024 * 1024);
            float  encoded_filesize_in_mb  = (float) encoded_filesize  / (1024 * 1024);
            float  compression_ratio       = (float) encoded_filesize  / original_filesize;
            char*  color = compression_ratio > 1.0f ? RED : GREEN;

            puts("");
            printf("Original filesize is: " BOLD "%.2fMB\n" RESET, original_filesize_in_mb);
            printf("Endoded filesize is:  " BOLD "%.2fMB\n" RESET, encoded_filesize_in_mb);
            printf("Compression ratio:    " "%s" "%.1f%%\n" RESET, color, compression_ratio * 100);
            puts("");
        }
    } else if (flags->decode) {
        char* in_filename = argv[flags->input_i];
        decode(in_filename);
        printf(GREEN "\nSuccess!\n" RESET);
    }


    free(flags);
    return 0;
}
