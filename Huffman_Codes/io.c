#include "io.h"
#include "defines.h"

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h> // for debugging

uint64_t bytes_read = 0;
uint64_t bytes_written = 0;

static uint8_t buf[BLOCK] = { 0 };
static int index = 0;

// function loops calls to read in order to make sure all bytes are read
// input: infile file descriptor, uint8_t array for buffer, how many bytes to read
// output: how many bytes were read
int read_bytes(int infile, uint8_t *buf, int nbytes) { // inspired by Christian's section
    int bytes = 0; // how many bytes were read in this call
    int curr_read = 0;
    if (nbytes == 0) {
        return 0;
    }

    while ((curr_read = read(infile, buf + bytes, nbytes - bytes)) > 0) {
        bytes += curr_read;
        bytes_read += curr_read; // add to total bytes read
        if (bytes >= nbytes) {
            break;
        }
    }
    return bytes;
}

// function loops calls to write to make sure all bytes are written
// input: outfile file descriptor, uint8_t array for buffer, how many bytes to read
// output: how many bytes were written
int write_bytes(int outfile, uint8_t *buf, int nbytes) { // inspired by Chirstian's section
    int bytes = 0; // how many bytes were written during this call
    int curr_write = 0;
    if (nbytes == 0) {
        return 0;
    }

    while ((curr_write = write(outfile, buf + bytes, nbytes - bytes)) > 0) {
        bytes += curr_write;
        bytes_written += curr_write; // add to total bytes written
        if (bytes >= nbytes) {
            break;
        }
    }
    return bytes;
}

// function reads from file one bit at a time
// input: infile file descriptor, pointer to a uint8_t bit (to take the bit)
// output: bool returns true if there are more bytes to read
bool read_bit(int infile, uint8_t *bit) { // inspired by Eugene's code! (Eugene's read_byte)
    static uint8_t buffer[BLOCK];
    static int index = 0; // tracks bit position in buffer
    static int end = -1; // tracks last valid byte

    if (index == 0) { // buffer is empty
        int bytes = read_bytes(infile, buffer, BLOCK);
        if (bytes < BLOCK) {
            end = (bytes * 8) + 1;
        }
    }

    if (((buffer[index / 8] >> index % 8) & 0x1)
        == 0x1) { // getting the bit of that particular byte
        *bit = 1;
    } else {
        *bit = 0;
    }
    index++;
    if (index == (BLOCK * 8)) {
        index = 0; // we have reached the end of the buffer
    }

    return (index != end);
}

// function writes bits into the buffer
// input: outfile file descriptor, a pointer to a code
// output: none (writes bit to outfile)
void write_code(int outfile, Code *c) {
    for (uint32_t i = 0; i < code_size(c); i++) {
        bool bit = code_get_bit(c, i);
        if (bit == true) {
            buf[index / 8] |= 0x1 << (index % 8); // sets bit at index to 1
        } else {
            buf[index / 8] &= ~(0x1 << (index % 8)); // sets bit at index to 0
        }
        index++;
        if (index >= (BLOCK * 8)) { // if we are at the end of the buffer!
            flush_codes(outfile);
            //index = 0; // go back to the start of the buffer
        }
    }
    return;
}

// function flushes left over bits in the buffer
// input: outfile file descriptor
// output: none
void flush_codes(int outfile) {
    if (index > 0) {
        int index_bytes = (index + 8 - 1) / 8; // ceil division!
        write_bytes(outfile, buf, index_bytes);
        index = 0;
    }
    return;
}
