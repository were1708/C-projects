#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>
#include <ctype.h>

#include "messages.h"
#include "bf.h"
#include "node.h"
#include "bst.h"
#include "ht.h"
#include "parser.h"

#define OPTIONS "hst:f:"
#define WORD    "[a-zA-Z0-9'-_]+"

int main(int argc, char **argv) {
    int opt = 0;
    bool help = false;
    bool stat = false;
    uint32_t ht_length = 65536;
    uint32_t bf_length = 1048576;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': help = true; break;
        case 's': stat = true; break;
        case 't': ht_length = atoi(optarg); break;
        case 'f': bf_length = atoi(optarg); break;
        }
    }

    if (help == true) {
        printf("SYNOPSIS\n  A word filtering program for the GPRSC.\n");
        printf("  Filters out and reports bad words parsed from stdin.\n\n");
        printf("USAGE\n  ./banhammer [-hs] [-t size] [-f size]\n\n");
        printf("OPTIONS\n");
        printf("  -h           Program usage and help.\n");
        printf("  -s           Print program statistics.\n");
        printf("  -t size      Specify hash table size (default: 2^16).\n");
        printf("  -f size      Specify Bloom filter size (default: 2^20).\n");
        exit(1);
    }

    HashTable *ht = ht_create(ht_length);
    BloomFilter *bf = bf_create(bf_length);
    FILE *badspeak = fopen("badspeak.txt", "r");
    FILE *newspeak = fopen("newspeak.txt", "r");
    if (badspeak == NULL || newspeak == NULL) {
        fprintf(stderr, "Error: couldn't open one or more files\n");
        exit(1);
    }
    char badword[1024];
    char oldword[1024];
    char newword[1024];
    while (fscanf(badspeak, "%s", badword) != EOF) {
        bf_insert(bf, badword);
        ht_insert(ht, badword, NULL);
    }
    while (fscanf(newspeak, "%s %s", oldword, newword) != EOF) {
        bf_insert(bf, oldword);
        ht_insert(ht, oldword, newword);
    }
    regex_t re;
    if (regcomp(&re, WORD, REG_EXTENDED)) { // this conditional based off of asgn7 pdf
        fprintf(stderr, "Failed to compile regex.\n");
        exit(1);
    }
    uint32_t string_index = 0; //this is for tolower
    char *word = NULL;
    Node *badspeak_root = bst_create();
    Node *newspeak_root = bst_create();
    // we are going to use binary search trees
    // to keep the list of words that are against
    // the rules.
    while ((word = next_word(stdin, &re)) != NULL) { // taken from asgn pdf
        while (word[string_index]) {
            word[string_index] = tolower(word[string_index]);
            string_index++;
        }
        string_index = 0; // reset back to 0
        if (bf_probe(bf, word)) { // if its maybe in the bloom filter
            Node *lookup = ht_lookup(ht, word);
            if (lookup != NULL) { // if its in the hashtable
                if (lookup->newspeak != NULL) {
                    newspeak_root = bst_insert(newspeak_root, lookup->oldspeak, lookup->newspeak);
                } else {
                    badspeak_root = bst_insert(badspeak_root, lookup->oldspeak, lookup->newspeak);
                }
            }
        }
    }

    if (stat == true) {
        printf("Average BST size: %lf\n", ht_avg_bst_size(ht));
        printf("Average BST height: %lf\n", ht_avg_bst_height(ht));
        printf("Average branches traversed: %lf\n", ((double) branches / lookups));
        printf("Hash table load: %lf%%\n", (100 * ((double) ht_count(ht) / ht_size(ht))));
        printf("Bloom filter load: %lf%%\n", (100 * ((double) bf_count(bf) / bf_size(bf))));
        clear_words();
        regfree(&re);
        fclose(badspeak);
        fclose(newspeak);
        bst_delete(&newspeak_root);
        bst_delete(&badspeak_root);
        ht_delete(&ht);
        bf_delete(&bf);
        // since we're exiting, we need to free everything!
        exit(1);
    }
    //printf("newspeak size: %u\nbadspeak size:%u\n", bst_size(newspeak_root),
    //   bst_size(badspeak_root) > 0);
    if (bst_size(newspeak_root) > 0 && bst_size(badspeak_root) > 0) { // mixspeak!
        printf("%s", mixspeak_message);
        bst_print(badspeak_root);
        bst_print(newspeak_root);
    } else if (bst_size(newspeak_root) > 0) {
        printf("%s", goodspeak_message);
        bst_print(newspeak_root);
    } else if (bst_size(badspeak_root) > 0) {
        printf("%s", badspeak_message);
        bst_print(badspeak_root);
    }

    clear_words();
    regfree(&re);
    fclose(badspeak);
    fclose(newspeak);
    bst_delete(&newspeak_root);
    bst_delete(&badspeak_root);
    ht_delete(&ht);
    bf_delete(&bf);
    return 0;
}
