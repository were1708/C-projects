#include <stdio.h>
#include <gmp.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>

#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

#define OPTIONS "hvb:i:n:d:s:"

int main(int argc, char **argv) {
    int opt = 0;
    bool help = false;
    bool verbose = false;
    uint64_t seed = time(NULL);
    uint64_t bits = 256;
    uint64_t iters = 50;
    char *pubfile = "rsa.pub";
    char *privfile = "rsa.priv";
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': help = true; break;
        case 'v': verbose = true; break;
        case 'b': bits = atoi(optarg); break;
        case 'i': iters = atoi(optarg); break;
        case 's': seed = atoi(optarg); break;
        case 'n': pubfile = optarg; break;
        case 'd': privfile = optarg; break;
        }
    }
    if (help == true) {
        printf("SYNOPSIS\n   Generates an RSA public/private key pair.\n\n");
        printf("USAGE\n   ./keygen [-hv] [-b bits] -n pbfile -d pvfile\n\n");
        printf("OPTIONS\n");
        printf("   -h              Display program help and usage.\n");
        printf("   -v              Display verbose program output.\n");
        printf("   -b bits         Minimum bits needed for public key n (default: 256).\n");
        printf("   -i confidence   Miller-Rabin iterations for testing primes (default: 50).\n");
        printf("   -n pbfile       Public key file (default: rsa.pub).\n");
        printf("   -d pvfile       Private key file (default: rsa.priv).\n");
        printf("   -s seed         Random seed for testing.\n");
        exit(1);
    }

    FILE *public_file = fopen(pubfile, "w");
    FILE *private_file = fopen(privfile, "w");

    if (public_file == NULL) {
        fprintf(stderr, "Error: could not open public file");
        exit(1);
    }
    if (private_file == NULL) {
        fprintf(stderr, "Error: could not open private file");
        exit(1);
    }

    int priv_fd = fileno(private_file);
    fchmod(priv_fd, 0600);

    randstate_init(seed);
    mpz_t p, q, n, e, d, s;
    mpz_inits(p, q, n, e, d, s, NULL);

    rsa_make_pub(p, q, n, e, bits, iters);
    rsa_make_priv(d, e, p, q);

    char *username = getenv("USER");
    mpz_set_str(s, username, 62);
    rsa_sign(s, s, d, n);

    rsa_write_pub(n, e, s, username, public_file);
    rsa_write_priv(n, d, private_file);

    if (verbose == true) {
        printf("user = %s\n", username);
        gmp_printf("s (%lu bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_printf("p (%lu bits) = %Zd\n", mpz_sizeinbase(p, 2), p);
        gmp_printf("q (%lu bits) = %Zd\n", mpz_sizeinbase(q, 2), s);
        gmp_printf("n (%lu bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("e (%lu bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
        gmp_printf("d (%lu bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    mpz_clears(p, q, n, e, d, s, NULL);
    randstate_clear();
    fclose(public_file);
    fclose(private_file);
    return 0;
}
