#include <stdio.h>
#include <gmp.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "numtheory.h"
#include "rsa.h"

#define OPTIONS "hvi:o:n:"

int main(int argc, char **argv) {
    int opt = 0;
    bool help = false;
    bool verbose = false;
    char *public_key_location = "rsa.pub"; // default location
    FILE *infile = stdin;
    FILE *outfile = stdout;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': help = true; break;
        case 'v': verbose = true; break;
        case 'n': public_key_location = optarg; break;
        case 'i': infile = fopen(optarg, "r"); break;
        case 'o': outfile = fopen(optarg, "w"); break;
        }
    }
    if (help == true) {
        printf("SYNOPSIS\n");
        printf("   Encrypts data using RSA encryption.\n");
        printf("   Encrypted data is decrypted by the decrypt program.\n\n");
        printf("USAGE\n   ./encrypt [-hv] [-i infile] [-o outfile] -n pubkey\n\n");
        printf("OPTIONS\n");
        printf("   -h              Display program help and usage.\n");
        printf("   -v              Display verbose program output.\n");
        printf("   -i infile       Input file of data to encrypt (default: stdin).\n");
        printf("   -o outfile      Output file for encrypted data (default: stdout).\n");
        printf("   -n pbfile       Public key file (default: rsa.pub).\n");
        exit(1);
    }

    FILE *public_key = fopen(public_key_location, "r");
    if (public_key == NULL) {
        fprintf(stderr, "Error: could not find public key");
        exit(1);
    }
    char username[LOGIN_NAME_MAX];
    mpz_t n, e, s, username_number;
    mpz_inits(n, e, s, username_number, NULL);
    rsa_read_pub(n, e, s, username, public_key);

    if (verbose == true) {
        printf("user = %s\n", username);
        gmp_printf("s (%lu bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_printf("n (%lu bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("e (%lu bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
    }

    mpz_set_str(username_number, username, 62);

    if (rsa_verify(username_number, s, e, n) == false) {
        fprintf(stderr, "Error: could not verify signature");
    }

    rsa_encrypt_file(infile, outfile, n, e);
    mpz_clears(n, e, s, username_number, NULL);
    if (infile != stdin) {
        fclose(infile);
    }
    if (outfile != stdout) {
        fclose(outfile);
    }

    fclose(public_key);
    return 0;
}
