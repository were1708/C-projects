#include "huffman.h"
#include "code.h"
#include "defines.h"
#include "node.h"
#include "pq.h"
#include "io.h"
#include "stack.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

// function builds a tree from a histagram
// and then returns the root node to the tree
// input: histagram of file
Node *build_tree(uint64_t hist[static ALPHABET]) {
    PriorityQueue *pq = pq_create(ALPHABET);
    for (int i = 0; i < ALPHABET; i++) { // this loop builds the pq with all the used bytes
        if (hist[i] > 0) {
            Node *current_node = node_create(i, hist[i]); // create a node with the symbol
            enqueue(pq, current_node); // put new node into the pq
        }
    }
    Node *left;
    Node *right;
    Node *parent;
    while (pq_size(pq) > 1) {
        dequeue(pq, &left);
        dequeue(pq, &right);
        parent = node_join(left, right);
        enqueue(pq, parent);
    }
    Node *root;
    dequeue(pq, &root);
    pq_delete(&pq);
    return root;
}

// (HELPER FUNCTION)
// function traverses tree and builds code for each byte
// input: root node of the tree, array to be filled , and a Code pointer
// output: none
void build_codes_helper(Node *root, Code table[static ALPHABET], Code c) {
    uint8_t bit_pop;
    if (root) {
        if (root->left == NULL && root->right == NULL) {
            table[root->symbol] = c;
        } else {
            code_push_bit(&c, 0);
            build_codes_helper(root->left, table, c);
            code_pop_bit(&c, &bit_pop);

            code_push_bit(&c, 1);
            build_codes_helper(root->right, table, c);
            code_pop_bit(&c, &bit_pop);
        }
    }
    return;
}

// function calls the helper function defined above
// initilizes a code to be used for building the code table
// input: root node and table for codes
void build_codes(Node *root, Code table[static ALPHABET]) {
    Code c = code_init();
    build_codes_helper(root, table, c);
}

// function writes the tree dump by traversing the tree
// when it sees a leaf node it writes an L and its symbol
// when it sees an internal node, it writes I
// input: outfile file descriptor and root node of tree
// output: none

static int index = 0;

void dump_tree_helper(int outfile, Node *root, uint8_t *buf, int *dump_size) {
    if (root) {
        dump_tree_helper(outfile, root->left, buf, dump_size);
        dump_tree_helper(outfile, root->right, buf, dump_size);

        if (root->left == NULL && root->right == NULL) { // if the node is a leaf node
            buf[index] = 76; // 'L'
            index++;
            *dump_size += 1;
            buf[index] = (root->symbol); // symbol of node
            index++;
            *dump_size += 1;
        } else {
            buf[index] = 73; // 'I'
            index++;
            *dump_size += 1;
        }
    }
    return;
}

void dump_tree(int outfile, Node *root) {
    uint8_t buf[MAX_TREE_SIZE] = { 0 };
    int dump_size = 0;
    dump_tree_helper(outfile, root, buf, &dump_size);
    write_bytes(outfile, buf, dump_size); // writes size of the tree dump
}

// function rebuilds the tree using a stack and the tree dump.
// input: size of tree dump, and tree dump
// output: pointer to root node of tree
Node *rebuild_tree(uint16_t nbytes, uint8_t tree[static nbytes]) {
    bool leaf = false;
    Node *right;
    Node *left;
    Node *parent;
    Stack *s = stack_create(nbytes);
    for (uint16_t i = 0; i < nbytes; i++) {
        if (leaf == true) {
            Node *n = node_create(tree[i], 0);
            stack_push(s, n);
            leaf = false;
        }

        else if (tree[i] == 76) { // 76 = 'L'
            leaf = true;
        } else if (tree[i] == 73) { // 73 = 'I'
            stack_pop(s, &right);
            stack_pop(s, &left);
            parent = node_join(left, right);
            stack_push(s, parent);
        }
    }
    stack_pop(s, &parent);
    stack_delete(&s);
    return parent;
}

// function deletes a tree
// input: pointer to a pointer to a root node
// output: none
void delete_tree(Node **root) {
    if (*root) {
        if ((*root)->right != NULL) {
            delete_tree(&(*root)->right);
        }
        if ((*root)->left != NULL) {
            delete_tree(&(*root)->left);
        }
        node_delete(root);
    }
    return;
}
