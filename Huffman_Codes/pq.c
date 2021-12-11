#include "pq.h"
#include "node.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct PriorityQueue {
    uint32_t head;
    uint32_t tail;
    uint32_t capacity;
    Node **items;
};

void swap(Node **a, Node **b) {
    Node *temp = *a;
    *a = *b;
    *b = temp;
    return;
}

// function finds the max child of a parent
// input: stat module, Array, first, and last element
// output: min child of a parent
uint32_t min_child(Node **A, uint32_t first, uint32_t last) {
    uint32_t left = 2 * first;
    uint32_t right = 2 * first + 1;
    if (right <= last && A[right - 1]->frequency < A[left - 1]->frequency) {
        return right;
    }
    return left;
}

// function: fixes a heap after the sort breaks it
// input: stat module, Array, first, and last element
// output: none, fixes heap
void fix_heap(Node **A, uint32_t first, uint32_t last) {
    bool found = false;
    uint32_t mother = first;
    uint32_t great = min_child(A, mother, last);

    while (mother <= last / 2 && !found == true) {
        if (A[mother - 1]->frequency > A[great - 1]->frequency) {
            swap(&A[mother - 1], &A[great - 1]);
            mother = great;
            great = min_child(A, mother, last);
        } else {
            found = true;
        }
    }
    return;
}

// Builds a heap from a given Array of Nodes
// input: array of node pointers, first and last (size)
// output: none
void build_heap(Node **A, uint32_t first, uint32_t last) {
    for (uint32_t father = last / 2; father > (first - 1); father--) {
        fix_heap(A, father, last);
    }
    return;
}

// function allocates memory for PriorityQueue and its array of nodes
// input: capacity of PriorityQueue
// output: pointer to a Priority Queue
PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *q = (PriorityQueue *) malloc(sizeof(PriorityQueue));
    if (q) {
        q->head = 0;
        q->tail = 0;
        q->capacity = capacity;
        q->items = (Node **) calloc(capacity, sizeof(Node *)); // pointer to node pointers
        if (!q->items) {
            free(q);
            q = NULL;
        }
    }
    return q;
}

// funtion deletes a PrioirityQueue and frees its allocated memory
// input: pointer to a pointer to a Priority Queue (to emulate pass by referenece)
// output: none
void pq_delete(PriorityQueue **q) {
    if (*q && (*q)->items) {
        free((*q)->items);
        free(*q);
        *q = NULL;
    }
    return;
}

// function returns true of PQ is empty
// input: pointer to a PQ
// output: bool type
bool pq_empty(PriorityQueue *q) {
    if (q->tail == 0) {
        return true;
    }
    return false;
}

// function returns true if PQ is full
// input: pointer to a PQ
// output: bool type
bool pq_full(PriorityQueue *q) {
    if (q->tail == q->capacity) {
        return true;
    }
    return false;
}

// function returns current size of PQ
// input: pointer to a PQ
// output: size of PQ as a uint32_t
uint32_t pq_size(PriorityQueue *q) {
    return q->tail;
}

// function enqueues an item into the PQ
// input: pointer to a PQ and a node to insert
// output: bool type indicating success
bool enqueue(PriorityQueue *q, Node *n) {
    if (q->tail == q->capacity) {
        return false;
    }
    q->items[q->tail] = n;
    q->tail++;
    build_heap(q->items, 1, q->tail);
    return true;
}

// function dequeues the smallest item (least frequency) from the PQ
// input: pointer to a PQ and a pointer to a node pointer
// output: bool type indicating success
bool dequeue(PriorityQueue *q, Node **n) {
    if (pq_empty(q) == true) {
        return false;
    }

    fix_heap(q->items, 1, q->tail);
    swap(&(q->items[0]), &(q->items[q->tail - 1]));
    *n = q->items[q->tail - 1]; // smallest element
    q->tail--;
    return true;
}

// prints a priorty queue (for debugging)
// input: pointer to a PQ
// output: returns nothing but prints the PQ
void pq_print(PriorityQueue *q) {
    for (uint32_t i = 0; i < q->tail; i++) {
        printf("%c\n", q->items[i]->symbol);
    }
    return;
}
