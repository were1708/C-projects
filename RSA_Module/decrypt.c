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
    char *private_key_location = "rsa.priv"; // default location
    FILE *infile = stdin;
    FILE *outfile = stdout;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': help = true; break;
        case 'v': verbose = true; break;
        case 'n': private_key_location = optarg; break;
        case 'o': outfile = fopen(optarg, "w"); break;
        case 'i': infile = fopen(optarg, "r"); break;
        }
    }
    if (help == true) {
        printf("SYNOPSIS\n");
        printf("   Decrypts data using RSA decryption.\n");
        printf("   Encrypted data is encrypted by the encrypt program.\n\n");
        printf("USAGE\n   ./decrypt [-hv] [-i infile] [-o outfile] -n privkey\n\n");
        printf("OPTIONS\n");
        printf("   -h              Display program help and usage.\n");
        printf("   -v              Display verbose program output.\n");
        printf("   -i infile       Input file of data to decrypt (default: stdin).\n");
        printf("   -o outfile      Output file for decrypted data (default: stdout).\n");
        printf("   -n pvfile       Private key file (default: rsa.priv).\n");
        exit(1);
    }
    FILE *private_key = fopen(private_key_location, "r");
    if (private_key == NULL) {
        fprintf(stderr, "Error: could not find private key");
        exit(1);
    }
    mpz_t n, d;
    mpz_inits(n, d, NULL);
    rsa_read_priv(n, d, private_key);
    if (verbose == true) {
        gmp_printf("n (%lu bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("d (%lu bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    rsa_decrypt_file(infile, outfile, n, d);

    mpz_clears(n, d, NULL);

    if (infile != stdin) {
        fclose(infile);
    }
    if (outfile != stdout) {
        fclose(outfile);
    }

    fclose(private_key);

    return 0;
}
