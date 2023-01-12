#include "queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>

// structs! (private)

typedef struct queue {
    int size;
    int head;
    int tail;
    int capacity;
    int *array; // the bounded queue!
} queue;

// constructors & destructors:

queue *create_queue(int capacity) {
    queue *q = (queue *) malloc(sizeof(queue));
    if (q) {
        q->size = 0;
        q->head = 0;
        q->tail = 0;
        q->capacity = capacity;
        q->array = (int *) calloc(capacity, sizeof(int));
        if (!q->array) { // if calloc fails
            free(q);
            q = NULL;
        }
    }
    return q;
}

void delete_queue(queue **q) {
    if (*q && (*q)->array) {
        free((*q)->array);
        free(*q);
        *q = NULL;
    }
    return;
}

// access functions:

int size(queue *q) {
    return q->size;
}

bool empty(queue *q) {
    return (q->size == 0);
}

bool full(queue *q) {
    return (q->size == q->capacity);
}

// manipulation functions

void enqueue(queue *q, int element) {
    if (q->size == q->capacity) { // queue is full
        return;
    }
    q->array[q->tail] = element;
    q->tail = (q->tail + 1) % q->capacity;
    q->size++;
}

int dequeue(queue *q) {
    int returnValue;
    if (q->size <= 0) { // queue is empty;
        return INT_MIN; // return error!
    }
    returnValue = q->array[q->head];
    q->head = (q->head + 1) % q->capacity;
    q->size--;
    return returnValue;
}

void print_queue(queue *q) { // for debugging purposes!
    for (int i = 0; i < q->size; i++) {
        printf("%d\n", q->array[(q->head + i) % q->capacity]);
    }
    return;
}
