#include "bst.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

uint64_t branches = 0;

// function creates an empty tree
// input: none
// output: node pointer that is equal to NULL
Node *bst_create(void) {
    return NULL;
}

// function deletes a binary search tree by deleting all nodes
// input: pointer to the root of the bst
// output: none
void bst_delete(Node **root) {
    if (*root) {
        if ((*root)->left != NULL) {
            bst_delete(&(*root)->left);
        }
        if ((*root)->right != NULL) {
            bst_delete(&(*root)->right);
        }
        node_delete(root);
    }
    return;
}

// function returns the height of the binary search tree
// input: pointer to the root of the binary search tree
// output: height of the tree (uint32_t)
uint32_t bst_height(Node *root) {
    if (root == NULL) {
        return 0;
    } else {
        uint32_t left_height = bst_height(root->left);
        uint32_t right_height = bst_height(root->right);
        if (left_height >= right_height) {
            return 1 + left_height;
        } else {
            return 1 + right_height;
        }
    }
}

// function returns the amount of nodes in the bst
// input: pointer to the root of the binary search tree
// output: size of the tree (uint32_t)
uint32_t bst_size(Node *root) {
    if (root == NULL) {
        return 0;
    } else {
        return 1 + bst_size(root->right) + bst_size(root->left);
    }
}

// function finds a node with oldspeak in the bst
// input: pointer to the root of the bst.
// output: node pointer to the node being searched (NULL if not in tree)
Node *bst_find(Node *root, char *oldspeak) { // find based off of lecture 18 code by Eugene!
    if (root) {
        int x = strcmp(root->oldspeak, oldspeak);
        if (x < 0) {
            branches++;
            return bst_find(root->right, oldspeak);
        } else if (x > 0) {
            branches++;
            return bst_find(root->left, oldspeak);
        }
    }
    return root;
}

// function inserts a node into the binary search tree
// input: pointer to the root of the bst
// output: Node pointer to the root of the tree
Node *bst_insert(Node *root, char *oldspeak, char *newspeak) { // based off of Eugene's section!
    if (root == NULL) { // this is if the tree is empty!
        return node_create(oldspeak, newspeak);
    }

    int x = strcmp(root->oldspeak, oldspeak);

    if (x == 0) { // for checking dupplicates
        return root;
    }

    if (x < 0) {
        branches++;
        root->right = bst_insert(root->right, oldspeak, newspeak);
    }

    if (x > 0) {
        branches++;
        root->left = bst_insert(root->left, oldspeak, newspeak);
    }
    return root;
}

// function prints the tree in order
// input: pointer to the root of the tree
// return: none
void bst_print(Node *root) { // based off of inorder_print from tree lecture 18
    if (root) {
        bst_print(root->left);
        node_print(root);
        bst_print(root->right);
    }
    return;
}
