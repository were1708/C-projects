CC = clang
CFLAGS = -Wall -Werror -Wextra -Wpedantic
LPFLAGS = -pthread

all: httpserver

httpserver: httpserver.o
	$(CC) -o httpserver httpserver.c io.c request.c List.c node.c parse.c queue.c -g $(LPFLAGS)

httpserver.o: httpserver.c io.c request.c List.c node.c parse.c
	$(CC) $(CFLAGS) -c $^ -g

queue.o: queue.c
	$(CC) $(CFLAGS) -c queue.c -g

io.o: io.c
	$(CC) $(CFLAGS) -c io.c -g

request.o: request.c List.c
	$(CC) $(CFLAGS) -c $^ -g

parse.o: parse.c
	$(CC) $(CFLAGS) -c parse.c -g

List.o: List.c node.c
	$(CC) $(CFLAGS) -c List.c -g

node.o: node.c
	$(CC) $(CFLAGS) -c node.c -g

format: 
	clang-format -i -style=file *.c *.h

clean:
	rm -rf *.o httpserver