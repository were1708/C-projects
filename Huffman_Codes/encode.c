#include "huffman.h"
#include "node.h"
#include "defines.h"
#include "io.h"
#include "header.h"

#include <stdint.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#define OPTIONS "hvi:o:"

// main file for encode!
int main(int argc, char **argv) {
    int opt = 0;
    bool help = false;
    bool verbose = false;
    int infile = STDIN_FILENO; // default infile is stdin
    int outfile = STDOUT_FILENO; // default outfile is stdout
    //bool temp_file = false;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': help = true; break;
        case 'v': verbose = true; break;
        case 'i': infile = open(optarg, O_RDONLY); break;
        case 'o': outfile = open(optarg, O_WRONLY | O_CREAT | O_TRUNC); break;
        }
    }

    if (help == true) {
        printf("SYNOPSIS\n");
        printf("  A Huffman encoder.\n  Compresses a file using the Huffman coding algorithm.\n\n");
        printf("USAGE\n  ./encode [-h] [-i infile] [-o outfile]\n\n");
        printf("OPTIONS\n");
        printf("  -h             Program usage and help.\n");
        printf("  -v             Print compression statistics.\n");
        printf("  -i infile      Input file to compress (infile required)\n");
        printf("  -o outfile     Output of compressed data.\n");
        exit(1);
    }

    if (infile == STDIN_FILENO) {
        fprintf(stderr, "Error: encoder needs infile.\n");
	fprintf(stderr, "use './encode -h' for options\n");
	exit(1);
    }

    uint8_t buf[BLOCK];
    struct stat size; // for collecting statistics on infile
    fstat(infile, &size);
    uint64_t file_size = size.st_size;
    uint16_t file_perm = size.st_mode;
    uint64_t bytes_to_read;
    uint64_t hist[ALPHABET] = { 0 }; // for frequencies
    hist[0] = 1;
    hist[255] = 1;
    uint32_t unique_symbols = 2; // counts unique symbols in file
    // bytes_read is a global variable from io!
    while (bytes_read < file_size) { // you want to read the whole file don't you?
        bytes_to_read = file_size - bytes_read; // how many bytes left to read!
        if (bytes_to_read >= BLOCK) {
            read_bytes(infile, buf, BLOCK);
            for (uint64_t i = 0; i < BLOCK; i++) { // loop through the buffer with size BLOCK
                hist[buf[i]] += 1; // find byte, increase it's count in hist
                if (hist[buf[i]] == 1) {
                    unique_symbols++;
                } // found a uniqie symbol!
            }
        } else {
            read_bytes(infile, buf, bytes_to_read); // less than a block left, read the rest!
            for (uint64_t i = 0; i < bytes_to_read; i++) {
                hist[buf[i]] += 1;
                if (hist[buf[i]] == 1) {
                    unique_symbols++;
                } // found a uniqie symbol!
            }
        }
    }
    uint16_t tree_size = (unique_symbols * 3) - 1;

    Header head = { .magic = MAGIC,
        .tree_size = tree_size, // sets the header!
        .file_size = file_size,
        .permissions = file_perm };

    fchmod(outfile, file_perm);

    write_bytes(outfile, (uint8_t *) &head, sizeof(Header)); // writes header to outfile!

    Code table[ALPHABET] = { 0 }; // store compressed codes in here

    Node *root = build_tree(hist);
    build_codes(root, table);
    dump_tree(outfile, root);
    delete_tree(&root);

    lseek(infile, SEEK_SET, 0);
    while (bytes_read < file_size * 2) { // assuming the whole file was already read
        bytes_to_read = (file_size * 2) - bytes_read; // how many bytes left to read!
        if (bytes_to_read >= BLOCK) {
            read_bytes(infile, buf, BLOCK);
            for (uint64_t i = 0; i < BLOCK; i++) { // loop through the buffer with size BLOCK
                write_code(outfile, &table[buf[i]]);
            }
        } else {
            read_bytes(infile, buf, bytes_to_read); // less than a block left, read the rest!
            for (uint64_t i = 0; i < bytes_to_read; i++) {
                write_code(outfile, &table[buf[i]]);
            }
            flush_codes(outfile);
        }
    }

    if (verbose) {
        double savings = 100 * (1 - ((double) bytes_written / (double) file_size));
        fprintf(stderr, "Uncompressed file size: %lu bytes\n", file_size);
        fprintf(stderr, "Compressed file size: %lu bytes\n", bytes_written);
        fprintf(stderr, "Space saving: %.2lf%%\n", savings);
    }

    if (infile != STDIN_FILENO) {
        close(infile);
    }
    //   if (temp_file == true) {
    //     remove("/tmp/tempfile.txt");
    //}
    if (outfile != STDOUT_FILENO) {
        close(outfile);
    }
    return 0;
}
