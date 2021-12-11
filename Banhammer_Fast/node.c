#include "node.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// function creates a node and allocates memory
// input: strings oldspeak and newspeak
// output: a Node pointer
Node *node_create(char *oldspeak, char *newspeak) {
    Node *n = (Node *) malloc(sizeof(Node));
    if (n) {
        n->left = NULL;
        n->right = NULL;
        n->oldspeak = strdup(oldspeak);
        if (newspeak != NULL) {
            n->newspeak = strdup(newspeak);
        } else {
            n->newspeak = NULL;
        }
    }
    return n;
}

// function deletes a node, frees all allocated memory!
// input: pointer to a node pointer
// output: none
void node_delete(Node **n) {
    if (*n) {
        free((*n)->oldspeak);
        free((*n)->newspeak);
        free(*n);
        *n = NULL;
    }
    return;
}

// function prints a node by printing its oldspeak to newspeak translation
// input: A Node pointer
// return val: None
void node_print(Node *n) {
    if (n) {
        if (n->newspeak != NULL) {
            printf("%s -> %s\n", n->oldspeak, n->newspeak);
        } else { // if no translation, just print the illegal word
            printf("%s\n", n->oldspeak);
        }
    }
    return;
}
