CC = clang
CFLAGS = -Wall -Werror -Wextra -Wpedantic

all: encode decode

encode: encode.o 
	$(CC) -o encode encode.c huffman.c code.c stack.c node.c pq.c io.c -g

decode: decode.o
	$(CC) -o decode decode.c huffman.c code.c stack.c node.c pq.c io.c -g

encode.o: encode.c io.c pq.c huffman.c node.c code.c
	$(CC) $(CFLAGS) -c $^

decode.o: decode.c io.c huffman.c code.c stack.c
	$(CC) $(CFLAGS) -c $^

node.o: node.c 
	$(CC) $(CFLAGS) -c node.c -g

pq.o: pq.c 
	$(CC) $(CFLAGS) -c pq.c -g

code.o: code.c 
	$(CC) $(CFLAGS) -c code.c -g

stack.o: stack.c 
	$(CC) $(CFLAGS) -c stack.c -g

io.o: io.c  
	$(CC) $(CFLAGS) -c io.c -g

huffman.o: huffman.c io.c pq.c node.c	
	$(CC) $(CFLAGS) -c $^ -g

format:
	clang-format -i -style=file *.c *.h

clean:
	rm -rf *.o encode decode

scan-build: clean
	scan-build make
