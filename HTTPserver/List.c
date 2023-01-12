#include "List.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Node *list_create(void) {
    return NULL;
}

void list_delete(Node **root) {
    if (*root) {
        Node *current = *root;
        Node *next_node;
        while (current != NULL) {
            next_node = current->next;
            node_delete(&current);
            current = next_node;
        }
        root = NULL;
    }
    return;
}

Node *list_find(Node *root, char *key) {
    if (root) {
        while (root) {
            if (!(strcmp(key, root->key))) {
                return root;
            }
            root = root->next;
        }
    }
    return root;
}

Node *list_insert(Node *root, char *key, char *val) {
    if (!root) {
        return node_create(key, val);
    }
    root->next = list_insert(root->next, key, val);
    return root;
}

int list_size(Node *root) {
    int size = 0;
    while (root) {
        size++;
        root = root->next;
    }
    return size;
}

void list_print(Node *root) {
    while (root) {
        node_print(root);
        root = root->next;
    }
    return;
}
