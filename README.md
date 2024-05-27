# Huffman Coding Project
This project is an implementation of the Huffman coding algorithm for file compression and decompression. It is written completely in C without any external libraries.

## Building
To build the project, compile and link either using `Tup`:
```bash
$ tup
```
or using `make`:
```bash
$ make all
```

## Usage
The `main` program supports the following modes:

### Encoding
To encode a file, run:
```bash
$ ./bin/main [filename]
```
or
```bash
$ ./bin/main [filename] [filename].huff
```

### Decoding
To decode a Huffman-encoded file, run:ლ
```bash
$ ./bin/main [filename].huff
```

### Demo
To encode a file and display information about the compression ratio, run:
```bash
$ ./bin/main [filename] demo
```

## File Structure
- `main.c` -> Command-line argument handling.<br>
- `IO.c` -> Functions for encoding and decoding files, reading and writing Huffman trees to files, etc.<br>
- `huffman.c` -> Functions for creating Huffman trees.<br>
- `heap.c` -> Utility functions for creating and manipulating heaps.<br>
- `Tupfile` -> Build system.<br>
- `files/` -> Example files to compress.<br>
