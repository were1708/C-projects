#pragma once
#include <stdbool.h>

typedef struct queue queue;

queue *create_queue(int capacity);

void delete_queue(queue **q);

int size(queue *q);

bool empty(queue *q);

bool full(queue *q);

void enqueue(queue *q, int element);

int dequeue(queue *q);

void print_queue(queue *q); // for debugging purposes
