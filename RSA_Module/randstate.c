#include "randstate.h"

gmp_randstate_t state;

// initilizes a random number in state
// input: takes a uint64_t seed
// output: none!
void randstate_init(uint64_t seed) {
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, seed);
    return;
}

// clears the state of the random number
// input: none
// output: none
void randstate_clear(void) {
    gmp_randclear(state);
    return;
}
