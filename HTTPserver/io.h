#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

int read_bytes(int infile, uint8_t *buf, int nbytes);

int write_bytes(int outfile, uint8_t *buf, int nbytes);
