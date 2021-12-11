# RSA Cryptography
#### Written by: Joshua Revilla

---

## Building
type `make` or `make all` to build keygen, encrypt, decrypt.
if you only want one of these programs, you can use
`make` and then the name of the program. For example,
`make keygen` will make the keygen program.

`make clean` deletes all compiler generated files.

`make format` formats all .c and .h files

---

## Running

for `keygen`: 

   ./keygen [-hv] [-b bits] -n pbfile -d pvfile

OPTIONS<br/>
   -h              Display program help and usage.<br/>
   -v              Display verbose program output.<br/>
   -b bits         Minimum bits needed for public key n.<br/>
   -c confidence   Miller-Rabin iterations for testing primes (default: 50).<br/>
   -n pbfile       Public key file (default: rsa.pub).<br/>
   -d pvfile       Private key file (default: rsa.priv).<br/>
   -s seed         Random seed for testing.<br/>

for `encrypt`:

   ./encrypt [-hv] [-i infile] [-o outfile] -n pubkey -d privkey

OPTIONS<br/>
   -h              Display program help and usage.<br/>
   -v              Display verbose program output.<br/>
   -i infile       Input file of data to encrypt (default: stdin).<br/>
   -o outfile      Output file for encrypted data (default: stdout).<br/>
   -n pbfile       Public key file (default: rsa.pub).<br/>

for `decrypt`:

   ./decrypt [-hv] [-i infile] [-o outfile] -n pubkey -d privkey

OPTIONS<br/>
   -h              Display program help and usage.<br/>
   -v              Display verbose program output.<br/>
   -i infile       Input file of data to decrypt (default: stdin).<br/>
   -o outfile      Output file for decrypted data (default: stdout).<br/>
   -d pvfile       Private key file (default: rsa.priv).<br/>


Please be sure to `make` all programs before trying to run them!

#### Dependencies

1. pkg-config
2. gmp (GNU Multiple Precision Arithmetic Library)
