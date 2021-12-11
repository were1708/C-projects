# Banhammer
#### Written by: Joshua Revilla

This program, takes in a list of badspeak, and oldspeak. It then parses
through a text file looking for instances of badspeak and oldspeak, if it finds
badspeak it will alert the user that they must be sent to `joycamp`. If it sees
any oldspeak, the program will instruct the user the newspeak translations to use 
instead.

`Big Brother` is ALWAYS watching!

---

## Building

type `make` in order to build the program `banhammer`
inputting `make banhammer` will also accomplish the same thing

`make format` will format all .c and .h files to the clang-format 
specifications.

`make clean` will delete all compiler generated files including
the binary `banhammer` and all other object files

be sure to make `banhammer` before trying to use it!

---

## Running 

`banhammer` can be run by using:
./banhammer [-hs] [-t size] [-f size]

OPTIONS<br/>
  -h           Program usage and help.<br/>
  -s           Print program statistics.<br/>
  -t size      Specify hash table size (default: 2^16).<br/>
  -f size      Specify Bloom filter size (default: 2^20).<br/>

be sure to `make` `banhammer` before trying to run it!

---


