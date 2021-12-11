#include "stack.h"

#include "node.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct Stack { // struct defintion taken from Prof. Long
    uint32_t top;
    uint32_t capacity;
    Node **items;
};

// creates a stack and allocates memory for the array of items
// input: capacity of stack
// output: stack
// (stack of node pointers)
Stack *stack_create(uint32_t capacity) { // function taken from Prof. Long
    Stack *s = (Stack *) malloc(sizeof(Stack));
    if (s) {
        s->top = 0;
        s->capacity = capacity;
        s->items = (Node **) calloc(capacity, sizeof(Node *));
        if (!s->items) {
            free(s);
            s = NULL;
        }
    }
    return s;
}

// deletes stack and frees memory allocated
// input: pointer to the stack pointer
// output: none
void stack_delete(Stack **s) { // function taken from Prof. Long
    if (*s && (*s)->items) {
        free((*s)->items);
        free(*s);
        *s = NULL;
    }
    return;
}

// returns number of elements in stack
// input: a stack
// output: amount of elements as a uint32_t
uint32_t stack_size(Stack *s) {
    return s->top;
}

// returns true if stack is empty, returns false otherwise
// input: a stack
// output: bool value telling if stack is empty
bool stack_empty(Stack *s) {
    return s->top == 0;
}

// returns true if stack is full returns false otherwise
// input: a stack
// output: bool value true if stack is full
bool stack_full(Stack *s) {
    return s->top == s->capacity;
}

// pushes x to top of stack
// input: Stack and uint32_t x
// output: bool informing if element was pushed
bool stack_push(Stack *s, Node *n) {
    if (s->top == s->capacity) { // stack full
        return false; // couldn't push
    } else {
        s->items[s->top] = n; // set the next available spot to x
        s->top++; // increment top of stack
        return true; // success!
    }
}

// pops element off of stack and stores sets it to x
// input: Stack and pointer to x
// output: bool indicating success or failure x now points at the element that was popped off
bool stack_pop(Stack *s, Node **n) {
    if (s->top == 0) { // stack was empty cant pop
        return false; //failure
    } else {
        *n = s->items[s->top - 1];
        s->top--; // top now points at a lower spot in memory
        return true; // success!
    }
}

// prints stack using the names of cities from graph
// input: Stack, output file, list of city names for printing!
// output: returns nothing but prints the Stack inputted
void stack_print(Stack *s) {
    for (uint32_t i = 0; i < s->top; i += 1) {
        printf("%c", s->items[i]->symbol);
    }

    printf("\n");
    return;
}
