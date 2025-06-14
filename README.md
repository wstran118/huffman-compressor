# File Compression Utility (Huffman Coding)
## Overview
This is a command-line file compression utility implemented in C using the Huffman coding algorithm. It compresses text files into a compact binary format and decompresses them back to their original form. The project demonstrates proficiency in data structures (binary trees, min-heaps), memory management, file I/O, and algorithm optimization—skills highly relevant to software engineering roles.

## Features

- Compression: Encodes text files using variable-length Huffman codes based on character frequencies.
- Decompression: Restores compressed files to their original content.
- Command-Line Interface: Supports easy-to-use commands for compression and decompression.
- Error Handling: Robust checks for file access, memory allocation, and invalid inputs.
- Performance Metrics: Achieves significant size reduction for repetitive text (see performance section).

## How It Works
Huffman coding assigns shorter binary codes to frequently occurring characters, reducing the overall file size. The program:

1. Builds a frequency table for characters in the input file.
2. Constructs a Huffman tree using a min-heap.
3. Generates prefix codes for each character.
4. Compresses the input by replacing characters with their codes and stores the frequency table for decompression.
5. Decompresses by rebuilding the Huffman tree and decoding the binary data.

## Installation
### Prerequisites

- GCC compiler
- Linux, macOS, or Windows with a C development environment

Clone the repository and compile the code:
````
git clone <repository-url>
cd <repository-directory>
gcc -o huff huffman.c
````

## Usage
Run the program with the following commands:

Compress a file: 
````
./huff -c input.txt output.huff
````

Decompress a file:
````
./huff -d output.huff restored.txt
````

#
