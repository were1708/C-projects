#pragma once

#include "node.h"
#include <stdbool.h>
#include <stdint.h>

Node *list_create(void);

int list_size(Node *root);

Node *list_find(Node *root, char *key);

Node *list_insert(Node *root, char *key, char *val);

void list_print(Node *root);

void list_delete(Node **root);
