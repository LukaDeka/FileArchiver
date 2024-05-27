#include <string.h>
#include <stdbool.h>
#include "main.h"
#include "IO.h"

bool has_huff_extension(char* arg) {
    char* extension = strrchr(arg, '.');
    if (extension == NULL) { return false; }
    return strcmp(strrchr(arg, '.'), ".huff") ? false : true;
}

bool has_demo_flag(char* arg) {
    return strcmp(arg, "demo") ? false : true;
}

size_t get_filesize(char* filename) {
    FILE* fp = fopen(filename, "rb");
    fseek(fp, 0, SEEK_END);
    size_t filesize = ftell(fp);
    fclose(fp);
    return filesize;
}

/*
    USAGE:
        TO ENCODE:
            ./main image.bmp
            ./main text.txt output_file_name.huff
        TO DECODE:
            ./main file.huff
        TO DISPLAY INFO ABOUT COMPRESSION:
            ./main voice.wav demo
    NOTES:
        The default output name is file.huff
        To decode, the file extension MUST be .huff
*/
int main(int argc, char* argv[]) {
    if (argc == 1) {
        printf("Please provide a filename.\n");
        return 1;
    } else if (argc == 2) {
        if (has_huff_extension(argv[1])) {
            decode(argv[1]);
        } else {
            encode(argv[1], "file.huff");
        }
    } else if (argc == 3 && !has_huff_extension(argv[1]) &&
                             has_huff_extension(argv[2])) {
        encode(argv[1], argv[2]);
    } else if (argc == 3 && !has_huff_extension(argv[1]) &&
                             has_demo_flag     (argv[2])) {
        encode(argv[1], "file.huff");

        size_t original_filesize = get_filesize(argv[1]);
        float  original_filesize_in_mb = (float) original_filesize / (1024 * 1024);
        size_t encoded_filesize  = get_filesize("file.huff");
        float  encoded_filesize_in_mb = (float) encoded_filesize / (1024 * 1024);
        float compression_ratio = (float) encoded_filesize / original_filesize;
        char* color = compression_ratio > 1.0f ? RED : GREEN;

        puts("");
        printf("Original filesize is: " BOLD "%.2fMB\n" RESET, original_filesize_in_mb);
        printf("Endoded filesize is:  " BOLD "%.2fMB\n" RESET, encoded_filesize_in_mb);
        printf("Compression ratio:    " "%s" "%.1f%%\n" RESET, color, compression_ratio * 100);
        puts("");
    } else {
        printf("Invalid format. Please refer to README.md.\n");
        return 1;
    }

    return 0;
}
