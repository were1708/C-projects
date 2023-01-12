#include "node.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// function creates a node and allocates memory
// input: string key, int val
// output: a Node pointer
Node *node_create(char *key, char *val) {
    Node *n = (Node *) malloc(sizeof(Node));
    if (n) {
        n->next = NULL;
        n->key = strdup(key);
        n->val = strdup(val);
    }
    return n;
}

// function deletes a node, frees all allocated memory!
// input: pointer to a node pointer
// output: none
void node_delete(Node **n) {
    if (*n) {
        free((*n)->key);
        free((*n)->val);
        free(*n);
        *n = NULL;
    }
    return;
}

// function prints a node by printing its key and value
// input: A Node pointer
// return val: None
void node_print(Node *n) {
    if (n) {
        printf("%s: %s\n", n->key, n->val);
    }
    return;
}
