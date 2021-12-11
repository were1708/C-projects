CC = clang
CFLAGS = -Wall -Werror -Wextra -Wpedantic $(shell pkg-config --cflags gmp)
LFLAGS = $(shell pkg-config --libs gmp)

all: keygen encrypt decrypt


decrypt: decrypt.o
	$(CC) -o decrypt decrypt.o numtheory.c randstate.c rsa.c $(LFLAGS)

encrypt: encrypt.o 
	$(CC) -o encrypt encrypt.c numtheory.c randstate.c rsa.c $(LFLAGS)

keygen: keygen.o 
	$(CC) -o keygen keygen.c numtheory.c randstate.c rsa.c $(LFLAGS)

decrypt.o: decrypt.c numtheory.c randstate.c rsa.c
	$(CC) $(CFLAGS) -c $^ 

encrypt.o: encrypt.c numtheory.c randstate.c rsa.c
	$(CC) $(CFLAGS) -c $^ 

keygen.o: keygen.c numtheory.c randstate.c rsa.c
	$(CC) $(CFLAGS) -c $^ 

rsa.o: rsa.c numtheory.c randstate.c
	$(CC) $(CFLAGS) -c $^ 

randstate.o: randstate.c
	$(CC) $(CFLAGS) -c randstate.c 

numtheory.o: numtheory.c randstate.c
	$(CC) $(CFLAGS) -c $^ 

clean:
	rm -rf *.o keygen encrypt decrypt

format:
	clang-format -i -style=file *.c *.h

scan-build: clean
	scan-build make
