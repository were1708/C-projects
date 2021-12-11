#include "bv.h"
#include <stdlib.h>
#include <stdio.h>
struct BitVector {
    uint32_t length;
    uint8_t *vector;
};

// function creates a bit vector and allocates memory for ADT
// input: length of the vector
// output: a pointer to a bitvector
BitVector *bv_create(uint32_t length) {
    BitVector *bv = (BitVector *) malloc(sizeof(BitVector));
    if (bv) {
        bv->length = length;
        bv->vector = (uint8_t *) calloc((length + 8 - 1) / 8, sizeof(uint8_t));
        // that math there is for seeing how many bytes you need to store
        // length amount of bits (ceil division!)
    }
    return bv;
}

// function deletes a bit vector and frees all allocated memory for the vector
// input: pointer to a bitvector pointer
// output: none
void bv_delete(BitVector **bv) {
    if (*bv && (*bv)->vector) {
        free((*bv)->vector);
        free(*bv);
    }
    *bv = NULL;
    return;
}

// function returns the length of a bitvector
// input: bitvector pointer
// output: length (uint32_t)
inline uint32_t bv_length(BitVector *bv) {
    return bv->length;
}

// function sets bit at i to 1
// input: bitvector pointer bv and index i
// output: bool indicating success
bool bv_set_bit(BitVector *bv, uint32_t i) {
    if (i > bv->length) {
        return false;
    }
    bv->vector[i / 8] |= (0x1 << (i % 8));
    return true;
}

// function clears bit at i to 0
// input: bitvector pointer bv and index i
// output: bool indicating success
bool bv_clr_bit(BitVector *bv, uint32_t i) {
    if (i > bv->length) {
        return false;
    }
    bv->vector[i / 8] &= ~(0x1 << (i % 8));
    return true;
}

// function checks bit at index i
// input: bitvector pointer and index i
// output: bool: true if 1 false if 0 or out of range
inline bool bv_get_bit(BitVector *bv, uint32_t i) {
    if (i >= bv->length) {
        return false;
    }
    if ((bv->vector[i / 8] >> (i % 8) & 0x1) == 0x1) { // one liner inspired by bv8.h
        return true;
    }
    return false;
}

// function prints all bits in the vector
// input: bitvector pointer
// return: none
void bv_print(BitVector *bv) {
    for (uint32_t i = 0; i < bv->length; i++) {
        printf("%d\n", bv_get_bit(bv, i));
    }
    return;
}
