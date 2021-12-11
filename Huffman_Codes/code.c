#include "code.h"
#include "defines.h"

#include <stdint.h>
#include <stdio.h>

// function intilizes a code
// input: none
// output: a Code
Code code_init(void) { // taken from Christian's Section
    Code c = { .top = 0, .bits = { 0 } };
    return c;
}

// function returns size of the code
// input: pointer to code
// outpuit: size of code(uint32_t)
uint32_t code_size(Code *c) {
    return c->top;
}

// function tells if code is empty
// input: pointer to a code
// output: boolean true if empty
bool code_empty(Code *c) {
    if (c->top == 0) {
        return true;
    }
    return false;
}

// function tells if code is full
// input: pointer to a code
// output: boolean true if code full
bool code_full(Code *c) {
    if (c->top >= MAX_CODE_SIZE) {
        return true;
    }
    return false;
}

// function sets a bit to 1
// input: pointer to a code and an index for the bit to set
// output: boolean true if successful
bool code_set_bit(Code *c, uint32_t i) { // inspired by bv8.h and Christian's section
    if (i > ALPHABET) {
        return false;
    }
    c->bits[i / 8] |= (0x1 << i % 8);
    return true;
}

// function clears a bit to 0
// input: pointer to a code and an index for the bit to clear
// output: boolean true if successful
bool code_clr_bit(Code *c, uint32_t i) {
    if (i > ALPHABET) {
        return false;
    }
    c->bits[i / 8] &= ~(0x1 << i % 8);
    return true;
}

// function gets a bit at index i
// input: pointer to code, and index i
// output: boolean (true if 1 false if 0)
// note: also returns false if i is out of range
bool code_get_bit(Code *c, uint32_t i) {
    if (((c->bits[i / 8] >> i % 8) & 0x1) == 0x1 && i <= ALPHABET) {
        return true;
    }
    return false;
}

// function pushes a bit onto the code stack
// input: a pointer to a code and a bit to push
// output: boolean true if successful
bool code_push_bit(Code *c, uint8_t bit) {
    if (code_full(c) == true) {
        return false;
    }
    if (bit == 0) {
        code_clr_bit(c, c->top);
    } else if (bit == 1) {
        code_set_bit(c, c->top);
    } else {
        return false; // what are you trying to push? not a 1 or a 0 that's for sure!
    }
    c->top++; // increment the top
    return true;
}

bool code_pop_bit(Code *c, uint8_t *bit) {
    if (code_empty(c) == true) {
        return false;
    }
    if (code_get_bit(c, c->top - 1) == true) {
        *bit = 1;
    } else {
        *bit = 0;
    }
    c->top--; // decrement top!
    return true;
}

void code_print(Code *c) {
    for (uint32_t i = 0; i < c->top; i++) {
        printf("%u ", code_get_bit(c, i));
    }
    printf("\n");
}
