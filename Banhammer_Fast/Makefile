CC = clang
CFLAGS = -Wall -Werror -Wextra -Wpedantic 

all: banhammer

banhammer: banhammer.o
	$(CC) -o banhammer banhammer.c bf.c bv.c node.c speck.c bst.c ht.c parser.c

banhammer.o: banhammer.c node.c bf.c bv.c speck.c bst.c ht.c parser.c
	$(CC) $(CFLAGS) -c $^ 

ht.o: ht.c bst.c
	$(CC) $(CFLAGS) -c ht.c 

parser.o: parser.c
	$(CC) $(CFLAGS) -c parser.c

bst.o: bst.c node.c
	$(CC) $(CFLAGS) -c bst.c 

bf.o: bf.c bv.c
	$(CC) $(CFLAGS) -c bf.c 

bv.o: bv.c
	$(CC) $(CFLAGS) -c bv.c 

speck.o: speck.c
	$(CC) $(CFLAGS) -c speck.c 

node.o: node.c
	$(CC) $(CFLAGS) -c node.c 

format:
	clang-format -i -style=file *c *.h

clean:
	rm -rf *.o banhammer

scan-build: clean
	scan-build make

