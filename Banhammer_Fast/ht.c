#include "ht.h"
#include "speck.h"
#include "node.h"
#include "salts.h"

#include <stdlib.h>
#include <stdio.h>

uint64_t lookups = 0;

struct HashTable {
    uint64_t salt[2];
    uint32_t size;
    Node **trees;
};

// creates a hashtable and allocates memory
// input: size of the table
// output: pointer to the hashtable
HashTable *ht_create(uint32_t size) {
    HashTable *ht = (HashTable *) malloc(sizeof(HashTable));
    if (ht) {
        ht->size = size;
        ht->salt[0] = SALT_HASHTABLE_LO;
        ht->salt[1] = SALT_HASHTABLE_HI;
        ht->trees = (Node **) calloc(ht->size, sizeof(Node *));
    }
    return ht;
}

// function deletes the hashtable and all trees within
// input: pointer to a hashtable pointer
// output: none
void ht_delete(HashTable **ht) {
    for (uint32_t i = 0; i < (*ht)->size; i++) {
        if ((*ht)->trees[i] != NULL) {
            bst_delete(&(*ht)->trees[i]); // delete all trees in the hashtable
        }
    }
    free((*ht)->trees); // free the array
    free(*ht); // free the structure
    *ht = NULL;
    return;
}

// function returns the size of the hashtable
// input: pointer to a hashtable
// output: size of the table (uint32_t)
inline uint32_t ht_size(HashTable *ht) {
    return ht->size;
}

// function looks for a node in the table
// input: Hashtable pointer and string to look for in table
// output: Node pointer to node with oldspeak (NULL if not found)
Node *ht_lookup(HashTable *ht, char *oldspeak) {
    lookups++;
    return bst_find(ht->trees[hash(ht->salt, oldspeak) % ht->size], oldspeak);
}

// function inserts a node with oldspeak in the table
// input: hashtable pointer, oldspeak and newspeak
// output: none
void ht_insert(HashTable *ht, char *oldspeak, char *newspeak) {
    lookups++;
    uint32_t x = hash(ht->salt, oldspeak) % ht->size;
    // by putting the hash in a variable, we don't have to hash the same thing twice!
    ht->trees[x] = bst_insert(ht->trees[x], oldspeak, newspeak);
    return;
}

// counts all non NULL entries in the hashtable
// input: pointer to a hashtable
// output: count of non NULL entries in the table (uint32_t)
uint32_t ht_count(HashTable *ht) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < ht->size; i++) { // iterate throught the table
        if (ht->trees[i] != NULL) { // if the entry is not null, that's an entry!
            count++;
        }
    }
    return count;
}

// returns the average size of all trees in the table
// input: pointer to a hashtable
// output: average height of all trees (double)
double ht_avg_bst_size(HashTable *ht) {
    uint32_t count = 0;
    uint32_t total_size = 0;
    for (uint32_t i = 0; i < ht->size; i++) {
        if (ht->trees[i] != NULL) {
            total_size += bst_size(ht->trees[i]);
            count++;
        }
    }
    return (double) total_size / count;
}

// function returns the average height of all trees in the table
// input: pointer to a hashtable
// output: average height of all trees (double)
double ht_avg_bst_height(HashTable *ht) {
    uint32_t count = 0;
    uint32_t total_height = 0;
    for (uint32_t i = 0; i < ht->size; i++) {
        if (ht->trees[i] != NULL) {
            total_height += bst_height(ht->trees[i]);
            count++;
        }
    }
    return (double) total_height / count;
}

// prints all trees in the hashtable
// input: pointer to a hashtable
// return: none
void ht_print(HashTable *ht) {
    for (uint32_t i = 0; i < ht->size; i++) {
        if (ht->trees[i] != NULL) {
            printf("index %u\n:", i);
            bst_print(ht->trees[i]);
        }
    }
    return;
}
