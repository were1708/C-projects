#pragma once

typedef struct Node Node;

struct Node {
    char *key;
    char *val;
    Node *next;
};

Node *node_create(char *key, char *val);

void node_delete(Node **n);

void node_print(Node *n);
