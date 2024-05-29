# Huffman Coding Project
This project is an implementation of the Huffman coding algorithm for file compression and decompression. It is written completely in `C`, without any external libraries.

## Format
The information in the encoded `.huff` file is saved in this order:
1) The null-terminated filename
2) One byte buffer for the amount of padding bits (filled at the end)
3) The Huffman tree, using precisely `2559` bits every time
4) The information, encoding using said Huffman tree
> Note: 256 leaf nodes, each corresponding an ASCII-char, are always encoded<br>
> `2559` bits = `(8+1) * 256 + 255`

## Building
To build the project, compile and link either using `Tup`:
```bash
$ tup
```
or using `make`:
```bash
$ make main
```

## Usage
The `main` program auto-detects if you want to compress or decompress. It also supports the following flags:
- `-o, --output` -> Specify output filename
- `-i, --info` -> Prints compression ratio
- `-h, --help` -> Displays usage info

### Examples
To encode a file, run:
```bash
$ ./main files/bird.bmp -o archive.huff -i
```
Output:
```
Original filesize is: 7.71MB
Endoded filesize is:  2.22MB
Compression ratio:    28.9%
```

### Decoding
Similarly, to decode a `.huff` file, run:
```bash
$ ./main archive.huff
```

## File Structure
- `main.c` -> Command-line argument and error handling.<br>
- `IO.c` -> Encoding and decoding files, reading and writing Huffman trees, etc.<br>
- `huffman.c` -> Functions for creating Huffman trees.<br>
- `heap.c` -> Utility functions for creating and manipulating heaps.<br>
- `files/` -> Example files to compress.<br>
