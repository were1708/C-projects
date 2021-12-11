#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"
#include <stdlib.h>
#include <stdio.h>

// function creates a public key
// input: bit length for numbers(nbits) and amount of iterations for miller rabin (iters)
// output: primes p and q, their product n and public exponent e
void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
    uint64_t pbits = random() % (((2 * nbits) / 4)); // random from [0, 2 * bits / 4)
    pbits += (nbits / 4); // now from [nbits/4, 3 * bits / 4)
    uint64_t qbits = nbits - pbits;
    pbits++; // to ensure the length of n
    qbits++; // to ensure the length of n
    make_prime(p, pbits, iters);
    make_prime(q, qbits, iters);
    mpz_mul(n, p, q);
    mpz_t tp, tq, tn, gcd_var; // totient p q and n
    mpz_inits(tp, tq, tn, gcd_var, NULL);
    mpz_sub_ui(tp, p, 1);
    mpz_sub_ui(tq, q, 1);
    mpz_mul(tn, tq, tp); // totient of n
    do {
        mpz_urandomb(e, state, nbits - 1);
        gcd(gcd_var, e, tn);
    } while (mpz_cmp_ui(gcd_var, 1) != 0); // while e and np are not coprime

    mpz_clears(tp, tq, tn, gcd_var, NULL);
    // clear all temp variables used
    return;
}

// writes a public key to a file
// input: modulus n, exponent e, signature s, username, public key file
// output: none
void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n", n);
    gmp_fprintf(pbfile, "%Zx\n", e);
    gmp_fprintf(pbfile, "%Zx\n", s);
    fprintf(pbfile, "%s", username);

    return;
}

// reads a public key from a file
// input: input file
// output: mod n, exponent e, signature s, username
void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx", n);
    gmp_fscanf(pbfile, "%Zx", e);
    gmp_fscanf(pbfile, "%Zx", s);
    gmp_fscanf(pbfile, "%s", username);
    return;
}

// generates a private key
// input: exponent e, primes p and q
// output: private d
void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
    mpz_t tp, tq, tn;
    mpz_inits(tp, tq, tn, NULL);
    mpz_sub_ui(tp, p, 1);
    mpz_sub_ui(tq, q, 1);
    mpz_mul(tn, tp, tq);
    mod_inverse(d, e, tn);
    mpz_clears(tp, tq, tn, NULL);
    return;
}

// writes a private key to a file
// input: modulus n, private d, private key file
// output: none
void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n", n);
    gmp_fprintf(pvfile, "%Zx\n", d);
    return;
}

// reads a private key from a file
// input: private key file
// output: modulus n, private d
void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx", n);
    gmp_fscanf(pvfile, "%Zx", d);
    return;
}

// encypts a message m into cyphertext c
// input: message m, exponent e and modulus n
// output: cypher text c
void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
    pow_mod(c, m, e, n);
    return;
}

// encrypts a file and writes it to outfile
// input: infile, outfile, mod n and exponent e
// output: none
void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
    mpz_t m, c;
    mpz_inits(m, c, NULL);
    uint64_t k = (mpz_sizeinbase(n, 2) - 1) / 8;
    int64_t bytes;
    uint8_t *buf = (uint8_t *) calloc(k, sizeof(uint8_t));
    buf[0] = 0xFF; // prepend 0xFF to start of each block!
    while (1) {
        bytes = fread(buf + 1, sizeof(uint8_t), k - 1, infile);
        if (bytes == 0) { // if at the end of the file
            break;
        }
        mpz_import(m, bytes + 1, 1, sizeof(uint8_t), 1, 0, buf);
        rsa_encrypt(c, m, e, n);
        gmp_fprintf(outfile, "%Zx\n", c);
    }
    mpz_clears(m, c, NULL);
    free(buf);
    return;
}

// decrypts cyphertext c into message m
// input: cyphertext c, private d, and modulus n
// output: message m
void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
    pow_mod(m, c, d, n);
    return;
}

// function decrypts a encrypted file
// input: infile, outfile, modulus n, and private d
// output: none
void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
    mpz_t m, c;
    mpz_inits(m, c, NULL);
    uint64_t k = (mpz_sizeinbase(n, 2) - 1) / 8;
    int scan = 1;
    uint64_t bytes;
    uint8_t *buf = (uint8_t *) calloc(k, sizeof(uint8_t));
    while (1) {
        scan = gmp_fscanf(infile, "%Zx", c); // read in cyphertext
        if (scan == EOF) {
            break;
        }
        rsa_decrypt(m, c, d, n);
        mpz_export(buf, &bytes, 1, sizeof(uint8_t), 1, 0, m);
        fwrite(buf + 1, sizeof(uint8_t), bytes - 1, outfile);
    }
    mpz_clears(m, c, NULL);
    free(buf);
    return;
}

// function signs a message with private d
// input: message m, private d, modulus n
// output: signature s
void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
    pow_mod(s, m, d, n);
    return;
}

// verfies a signature
// input: message m, signature s, exponent e, modulus n
// output: bool indicating verification status
bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
    mpz_t t;
    mpz_init(t);
    pow_mod(t, s, e, n);
    if (mpz_cmp(t, m) == 0) {
        mpz_clear(t);
        return true;
    }
    mpz_clear(t);
    return false;
}
