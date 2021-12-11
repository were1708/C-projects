#include "node.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

// function creates a node ADT and allocates memory
// set children to NULL be default
// input: symbol and frequency for node
// output: pointer to a node
Node *node_create(uint8_t symbol, uint64_t frequency) {
    Node *n = (Node *) malloc(sizeof(Node));
    if (n) {
        n->symbol = symbol;
        n->frequency = frequency;
        n->left = NULL; // default node will be a leaf node
        n->right = NULL; // set these nodes to NULL, no children
    }
    return n;
}

// function deletes a node
// input: pointer to a pointer to a node
// output: none
void node_delete(Node **n) {
    if (*n) {
        free(*n);
        *n = NULL;
    }
    return;
}

// function joins two nodes to one parent node
// sets symbol to $ and frequency to right child + left child frequencies
// input: two pointers to two nodes
// output: pointer to parent node
Node *node_join(Node *left, Node *right) {
    uint8_t symbol = 36; // dollar sign!
    uint64_t frequency = left->frequency + right->frequency;
    Node *parent = node_create(symbol, frequency);
    if (parent) {
        parent->left = left;
        parent->right = right;
    }
    return parent;
}

// function prints node and all children
// (mostly for debugging and testing)
// input: pointer to a node
// output: no return val, prints node
void node_print(Node *n) {
    printf("%c\n", n->symbol);
    if (n->right != NULL) {
        node_print(n->right);
    }
    if (n->left != NULL) {
        node_print(n->left);
    }

    return;
}
