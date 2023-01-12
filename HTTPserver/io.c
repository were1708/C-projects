#include "io.h"

int read_bytes(int infile, uint8_t *buf, int nbytes) {
    int bytes = 0; // how many bytes were read in this call
    int curr_read = 0;
    if (nbytes == 0) {
        return 0;
    }

    while ((curr_read = read(infile, buf + bytes, nbytes - bytes)) > 0) {
        bytes += curr_read;
        if (bytes >= nbytes) {
            break;
        }
    }
    return bytes;
}

int write_bytes(int outfile, uint8_t *buf, int nbytes) {
    int bytes = 0; // how many bytes were written during this call
    int curr_write = 0;
    if (nbytes == 0) {
        return 0;
    }

    while ((curr_write = write(outfile, buf + bytes, nbytes - bytes)) > 0) {
        bytes += curr_write;
        if (bytes >= nbytes) {
            break;
        }
    }
    return bytes;
}
