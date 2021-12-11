# Huffman Coding
#### Written by: Joshua Revilla

This program takes a file and compresses it through `./encode`.
The program can also decompress this file through `./decode`.
This program uses the Huffman Coding algorithim created by
David Huffman (Go Slugs!). You can enable statisics by using `-v` 

---

## Building

type `make` or `make all` to build encode and decode.
if you only want one of these programs you can by using
`make encode` or `make decode`.

`make clean` will delete all compiler generated files

`make format` will `clang-format` all `.c` and `.h` files

---

## Running

for `encode`:

  ./encode [-h] [-i infile] [-o outfile]

OPTIONS<br/>
  -h             Program usage and help.<br/>
  -v             Print compression statistics.<br/>
  -i infile      Input file to compress. (infile required)<br/>
  -o outfile     Output of compressed data.<br/>

---

for `decode`:

  ./decode [-h] [-i infile] [-o outfile]<br/>

OPTIONS<br/>
  -h             Program usage and help.<br/>
  -v             Print compression statistics.<br/>
  -i infile      Input file to decompress. (infile required)<br/>
  -o outfile     Output of decompressed data.<br/>

Be sure to `make` each executable before running!


