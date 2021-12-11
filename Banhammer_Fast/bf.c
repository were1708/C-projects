#include "bf.h"
#include "salts.h"
#include "speck.h"
#include "bv.h"
#include <stdio.h>
#include <stdlib.h>

struct BloomFilter {
    uint64_t primary[2];
    uint64_t secondary[2];
    uint64_t tertiary[2];
    BitVector *filter;
};

// function creates a bloomfilter and allocates memory
// also sets the salts to the correct position
// input: size of filter
// output: bloomfilter pointer
BloomFilter *bf_create(uint32_t size) {
    BloomFilter *bf = (BloomFilter *) malloc(sizeof(BloomFilter));
    if (bf) {
        bf->primary[0] = SALT_PRIMARY_LO;
        bf->primary[1] = SALT_PRIMARY_HI;
        bf->secondary[0] = SALT_SECONDARY_LO;
        bf->secondary[1] = SALT_SECONDARY_HI;
        bf->tertiary[0] = SALT_TERTIARY_LO;
        bf->tertiary[1] = SALT_TERTIARY_HI;
        //BitVector *filter = bv_create(size);
        bf->filter = bv_create(size);
    }
    return bf;
}

// function deletes a bloom filter and frees allocated memory
// input: pointer to a bloomfilter pointer
// output: none
void bf_delete(BloomFilter **bf) {
    if (*bf && (*bf)->filter) {
        bv_delete(&(*bf)->filter);
        free(*bf);
        *bf = NULL;
    }
    return;
}

// function returns the size of the bloomfilter
// input: bloomfilter pointer
// output: size of filter (uint32_t)
uint32_t bf_size(BloomFilter *bf) {
    return bv_length(bf->filter);
}

// function inserts a word into the bloomfilter
// input: bloomfilter pointer and string to insert
// output: none
void bf_insert(BloomFilter *bf, char *oldspeak) {
    bv_set_bit(bf->filter, hash(bf->primary, oldspeak) % bv_length(bf->filter));
    bv_set_bit(bf->filter, hash(bf->secondary, oldspeak) % bv_length(bf->filter));
    bv_set_bit(bf->filter, hash(bf->tertiary, oldspeak) % bv_length(bf->filter));
    return;
}

// function checks if a word is in the bloomfilter (probabilistic)
// input: bloomfilter pointer and string to check
// output: false if string is FOR SURE not in the filter true if it's maybe in the filter
bool bf_probe(BloomFilter *bf, char *oldspeak) {
    bool primary_bool = bv_get_bit(bf->filter, hash(bf->primary, oldspeak) % bv_length(bf->filter));
    bool secondary_bool
        = bv_get_bit(bf->filter, hash(bf->secondary, oldspeak) % bv_length(bf->filter));
    bool tertiary_bool
        = bv_get_bit(bf->filter, hash(bf->tertiary, oldspeak) % bv_length(bf->filter));
    if ((primary_bool == true && secondary_bool == true && tertiary_bool == true)) {
        return true;
    }
    return false;
}

// function returns the amount of set bits in the filter
// input: bloomfilter pointer
// output: count of set bits (uint32_t)
uint32_t bf_count(BloomFilter *bf) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < bf_size(bf); i++) {
        if (bv_get_bit(bf->filter, i) == true) {
            count++;
        }
    }
    return count;
}

// prints out the vector associated
// with the bloom filter
// input: bloomfilter pointer
// return: none
void bf_print(BloomFilter *bf) {
    bv_print(bf->filter);
    return;
}
