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

Node *traverse_left(Node *n) {
    return n->left;
}

Node *traverse_right(Node *n) {
    return n->right;
}

int main(int argc, char **argv) {
    int opt = 0;
    bool help = false;
    bool verbose = false;
    int infile = STDIN_FILENO; // default infile is stdin
    int outfile = STDOUT_FILENO; // default outfile is stdout
    //    bool temp_file = false;
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
        printf(
            "  A Huffman decoder.\n  Decompresses a file using the Huffman coding algorithm.\n\n");
        printf("USAGE\n  ./decode [-h] [-i infile] [-o outfile]\n\n");
        printf("OPTIONS\n");
        printf("  -h             Program usage and help.\n");
        printf("  -v             Print compression statistics.\n");
        printf("  -i infile      Input file to decompress. (infile required)\n");
        printf("  -o outfile     Output of decompressed data.\n");
        exit(1);
    }
    
    if (infile == STDIN_FILENO) {
        fprintf(stderr, "Error: decoder needs infile.\n");
	fprintf(stderr, "use './decode -h' for options\n");
	exit(1);
    }
    
    uint8_t head[sizeof(Header)]; //buffer for the header

    read_bytes(infile, head, sizeof(Header)); // reading the Header!
    uint32_t magic_num = 0; // stores magic num from encoded file
    uint16_t file_permission = 0; // stores permission
    uint16_t tree_size = 0; // stores size of tree dump
    uint64_t original_file_size = 0; // size of original file

    for (int i = 0; i < 4; i++) { // this loop reads the magic number and puts it in magic_num
        magic_num += head[i] << (i * 8); // little endian! if big endian swap this order!
    }
    if (magic_num != MAGIC) { // checks for header for valid compressed file
        fprintf(stderr, "Error: unable to read header\n");
        exit(1);
    }

    for (int i = 4; i < 6; i++) {
        file_permission += head[i] << ((i - 4) * 8); // sets file permision
    }

    for (int i = 6; i < 8; i++) {
        tree_size += head[i] << ((i - 6) * 8); // sets file permision
    }

    for (int i = 8; i < 16; i++) {
        original_file_size += head[i] << ((i - 8) * 8);
    }

    fchmod(outfile, file_permission);

    uint8_t tree_dump[tree_size];
    read_bytes(infile, tree_dump, tree_size);
    Node *root = rebuild_tree(tree_size, tree_dump);
    Node *curr = root;
    uint8_t bit;
    uint8_t write_buf[BLOCK] = { 0 }; // buffer for writing to outfile!
    uint16_t index = 0; // for the write buffer
    uint64_t bytes_to_write;
    while (read_bit(infile, &bit) || bytes_written < original_file_size) {
        // while there are bits to read and we haven't hit the size of the original file!

        if (curr->right == NULL && curr->left == NULL) {
            write_buf[index] = curr->symbol;
            curr = root; // go back to the top of the tree!
            index++;
        }

        if (bytes_to_write < BLOCK && index == bytes_to_write) {
            break;
        }
        if (index == BLOCK) {
            write_bytes(outfile, write_buf, BLOCK);
            index = 0; // reset index of buffer!
        }

        if (bit == 0) {
            curr = traverse_left(curr);
        } else if (bit == 1) {
            curr = traverse_right(curr);
        }
        bytes_to_write = original_file_size - bytes_written; // update bytes to write
    }

    if (bytes_to_write > 0) {
        write_bytes(outfile, write_buf, bytes_to_write);
    }

    delete_tree(&root);

    if (verbose) {
        double savings = 100 * (1 - ((double) bytes_read / (double) original_file_size));
        fprintf(stderr, "Uncompressed file size: %lu bytes\n", original_file_size);
        fprintf(stderr, "Compressed file size: %lu bytes\n", bytes_read);
        fprintf(stderr, "Space saving: %.2lf%%\n", savings);
    }

    if (infile != STDIN_FILENO) {
        close(infile);
    }

    //if (temp_file == true) {
    //remove("/tmp/tempfile.txt");
    // }

    if (outfile != STDOUT_FILENO) {
        close(outfile);
    }
    return 0;
}
