#include <stdlib.h>
#include "numtheory.h"
#include "randstate.h"
#include <time.h>

//                  NOTE:
// make sure that randstate has been initilzied
// these functions will assume that it has been initilized
// so that it is easier to clear the state and to save
// time and memory!

// function computes the GCD of a and b
// and stores it inside of d
// input: mpz_t a and b and mpz_t d for
// out value. (call by reference)
// return value: none!
void gcd(mpz_t d, mpz_t a, mpz_t b) {
    mpz_t t, temp_a, temp_b;
    mpz_inits(t, temp_a, temp_b, NULL); // makes a temp value t
    mpz_set(temp_a, a);
    mpz_set(temp_b, b);
    while (mpz_cmp_ui(temp_b, 0) != 0) { // while b != 0
        mpz_set(t, temp_b);
        mpz_mod(temp_b, temp_a, temp_b);
        mpz_set(temp_a, t);
    }
    mpz_set(d, temp_a); // d is the out value!
    mpz_clears(t, temp_a, temp_b, NULL); // frees the temp value!
    return; // void function
}

// function calculates modular exponentiation!
// input: out variable, mpz_t base, exponent, and modulus
// value stored in mpz_t out
// return value: none
void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus) {
    mpz_t v, p, two, temp_e; // temp_e for temp exponent
    mpz_init(v);
    mpz_init(p);
    mpz_set(p, base);
    mpz_init_set(temp_e, exponent);
    mpz_set_ui(v, 1);
    mpz_init_set_ui(two, 2); // done for division later on (type match)
    while (mpz_cmp_ui(temp_e, 0) > 0) {
        if (mpz_odd_p(temp_e) != 0) {
            mpz_mul(v, v, p);
            mpz_mod(v, v, modulus);
        }
        mpz_mul(p, p, p);
        mpz_mod(p, p, modulus);
        mpz_fdiv_q(temp_e, temp_e, two);
    }
    mpz_set(out, v);
    mpz_clear(v);
    mpz_clear(p);
    mpz_clear(two);
    mpz_clear(temp_e);
    return;
}

// calculates the modular inverse of a mod n
// stores the value in i
// input: mpz_t i (out), base a, modulus n
// return value: none
void mod_inverse(mpz_t i, mpz_t a, mpz_t n) {
    mpz_t r, rn, t, tn, q, temp;
    mpz_inits(r, rn, t, tn, q, temp, NULL);
    mpz_set(r, n);
    mpz_set(rn, a);
    mpz_set_ui(t, 0);
    mpz_set_ui(tn, 1);
    while (mpz_cmp_ui(rn, 0) != 0) {
        mpz_fdiv_q(q, r, rn);
        mpz_set(temp, r);
        mpz_set(r, rn);
        mpz_mul(rn, q, rn);
        mpz_sub(rn, temp, rn);
        mpz_set(temp, t);
        mpz_set(t, tn);
        mpz_mul(tn, q, tn);
        mpz_sub(tn, temp, tn);
    }
    if (mpz_cmp_ui(r, 1) > 0) {
        mpz_clears(r, rn, t, tn, q, temp, NULL);
        mpz_set_ui(i, 0);
        return; // there is no inverse!
    }
    if (mpz_cmp_ui(t, 0) < 0) {
        mpz_add(t, t, n);
    }
    mpz_set(i, t);
    mpz_clears(r, rn, t, tn, q, temp, NULL);
    return;
}

// uses the Millan Rabin test to see if a
// number is prime.
// input: number to test and how many times to test it
// output: bool value true if prime.
bool is_prime(mpz_t n, uint64_t iters) {

    if (mpz_even_p(n) > 0 && mpz_cmp_ui(n, 2) != 0) { // if n is even and not 2
        return false; // the number is not prime!
    }

    if (mpz_cmp_ui(n, 0) == 0 || mpz_cmp_ui(n, 1) == 0) { // zero and one are not prime
        return false;
    }
    if (mpz_cmp_ui(n, 3) == 0 || mpz_cmp_ui(n, 2) == 0) { // if n is 3 or 2
        return true; // then n is prime!
    }

    // above checks if the number is even and not two since then it's
    // for sure not a prime number!

    mpz_t nmo, r, power; // n minus one and r value
    mpz_inits(nmo, r, power, NULL);
    mpz_sub_ui(nmo, n, 1);
    mpz_set(r, nmo);
    // sets everything up to achieve the correct r value!
    mpz_set_ui(power, 1); // 2^0 = 1
    uint64_t s = 0;
    while (1) { // loop infinitley
        mpz_fdiv_q(r, nmo, power);
        if (mpz_even_p(r) == 0) { // if n r is odd
            break;
        }
        mpz_mul_ui(power, power, 2); // multiply by 2 each iteration for 2^s
        s++;
    }
    // gmp_printf("%Zd\n", r);
    // printf("%lu\n", s);
    mpz_clear(power); // don't need power anymore!

    mpz_t a, y, j, two, nmt;
    mpz_inits(a, y, j, two, nmt, NULL);
    mpz_sub_ui(nmt, n, 4);
    mpz_set_ui(two, 2);
    for (uint64_t i = 0; i < iters; i++) {
        mpz_urandomm(a, state, nmt);
        mpz_add_ui(a, a, 2); // a is now a random num between [2, n-2]
        pow_mod(y, a, r, n);
        if (mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, nmo) != 0) {
            mpz_set_ui(j, 1);
            while (mpz_cmp_ui(j, s - 1) <= 0 && mpz_cmp(y, nmo) != 0) {
                pow_mod(y, y, two, n);
                if (mpz_cmp_ui(y, 1) == 0) {
                    mpz_clears(a, nmo, nmt, y, j, r, two, NULL);
                    // clear all used mpz_t
                    return false; // number is not prime
                }
                mpz_add_ui(j, j, 1);
            }

            if (mpz_cmp(y, nmo) != 0) {
                mpz_clears(a, nmo, nmt, y, j, r, two, NULL);
                // clear all used mpz_t
                return false;
            }
        }
    }
    mpz_clears(a, nmo, nmt, y, j, r, two, NULL);
    // clear all used mpz_t
    return true;
}

// function generates a prime number a prime number
// at least (bits) bits long and tests primality iters
// times
//
// input: mpz_t p (out), bits (length), iters (test iterations)
// return value: none
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    mpz_t lim;
    mpz_init_set_ui(lim, 0);
    mpz_setbit(lim, bits - 1); // add this to random num to achieve the correct amount of bits!
    do {
        mpz_urandomb(p, state, bits - 1);
        mpz_add(p, p, lim);

    } while (is_prime(p, iters) == false || mpz_sizeinbase(p, 2) < bits);

    mpz_clear(lim);
    return;
}
