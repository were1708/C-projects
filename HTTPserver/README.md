# HTTPserver (with Multi-threading!)
#### Written by Joshua Revilla

### Usage:
to start the `httpserver` build using `make`
this program requires a port number to function. 
here's the format:</br>
`./httpserver -t <threads> -l <logfile> <port number>`</br>
this server stays up and can recieve different kinds of requests
through its port number. it can accept `GET` requests,
`PUT` requests and `APPEND` requests. it can perform these
actions if the `request` was formatted correctly, so please 
be sure to format the request in the proper format.

### Summary:

So in order to build this server, I split the process into
a couple of different modules. the two first modules I'll cover 
is `node.c` and `List.c`. These modules constitute the `Linked List`
that I build in order to store the headers that are passed in throught the request.
`node` defines the structure of the node containing a pointer to the next node
and the `key` `value` pair that each header consists of. `List` handles
a lot of the `Linked List` functions like delete, search and insert. I used a linked list
due to the limited amount of headers, this means I wont have to search for much and 
inserting is faster! I also have a module for io called `io.c` which handles
looping calls to read and write sp that I can simply call `read_bytes()`
or `write_bytes` without having to worry about `read()` and `write()`
failing! I also wrote a module that handles parsing called `parse.c`. 
This module uses `Regex` in order to parse the request while also
splitting up the request line by line to make it easier to process.
the next module I decieded to build was my `request` module. This module
contains the implementation for `GET` `PUT` and `APPEND`. This module
relies heavily on `io` and `parse` in order to read/write appropriately.
it uses `parse` in order to recieve the correct arguements to conduct its
operations. `GET` simply checks a files permissions and if it exists, if it does
it then procedes to `read()` in bytes and then I use `write_bytes()` in order 
to write those bytes to the `socket`. `PUT` and `APPEND` are very similar because;
they both `write` to files. it takes the message from the `request` and writes it 
to the file specified by the `request`. `PUT` will truncate a file if it already exists
and create a file if one doesn't exist. `APPEND` will append to a file and will return a `404`
error if the file exists. A large part of this project was returning the correct messages.
here's what all messages from the program means:</br>

`200` OK</br><br>
`201` File Created! (for `PUT`)<br>
`400` Bad Request! (poorly formatted or not enough info!)</br><br>
`403` No Permissons (file was restricted or a directory)</br><br>
`404` File Not Found (for `GET` and `APPEND`)</br><br>
`500` Internal Server Error (something bad happened) </br><br>
`501` Not Implemented (Method was undefined!)</br><br>

The server reads in the request from the `socket` using `read_bytes`.
it then parses the request using the `parse` module. the modules gives 
us all the information we need like the Method and URI (file path). it also
puts all the header `key` `value` pairs into a `Linked List` like explained before.
the server then checks this data, and performs the correct request using the 
`request` module.


### Logging 
Logging was accomplished by searching in the linked list for `Request-Id` for the
ID of the request. If it is not found, the ID will be set to `0`. We take the 
request type, the `URI`, the status code, and the Request-Id and write that into a `logfile` that
is set to `stderr` by default. We already have all of this info from parsing the header, and 
from retrieving the return code from the method's function for the status code.
we use `fprintf()` in order to print to the `logfile` and every valid method call
is logged into the `logfile`. Not too much extra work had to be done in order to
enable loggin since we had most of the information we needed, we just needed to write
it to a file. Specify a `logfile` using the `-l` flag and indicate the number of threads
using the `-t` flag


### Threadpool
For this project I had to implement a threadpool in order to handle multiple requests at once
we needed a way to store the connections that came in and that was the first problem I tackled.
I implemented a bounded `Queue` with a size of 2048. I thought making it bounded would be an easier soultion
and hopefully more flexable since the array has all the elements next to each other in memory.
when the server recieves a connection it puts `connfd` into queue for one of the worker threads 
to take. The `Queue` was an easy and obvious solution to this problem. I made an global array of `pthread_t`
with size of `thread` which was how many threads the server should have. It's important to note how
`Mutual Exclusion` is achieved. This ensures two threads don't `dequeue` at the same time and 
corrupt the state of the `Queue`. I used a `pthread_mutex_t` in order to protect the queue.
A thread must grab the lock before they can `enqueue` or `dequeue`. I also used a `Conditional Variable`
in order to ensure that a thread will wait if the `Queue` has no connections for it, and so that
the dispatcher won't add to the `Queue` if it is full. When the dispatcherthread `enqueues` a connection,
it also signals a thread so that it can wake up, `dequeue` a connection and then handle it. 
this tells the thread that it should start wrapping itself up. The thread will see this value change and return.
the signal handler then calls `pthread_join` in order to clear the threads out. In this signal handler we also
must call `free()` on the `Queue`. In summary we protect the `Queue` with mutex locks and we avoid busy waiting
by using condition variables. This way, all the threads can be inactive when there's no work and can all run in 
parallel when there is a lot of work, this greatly increases throughput. If an invalid amount of threads was 
inputted then the server will exit with `EXIT_FAILURE`. At a high level, this server will have one thread spawn 
`t` amount of threads and then start listening to the socket. If it recieves a request it will `enqueue` the
`connfd` and signal a spawned worker thread. The worker thread will then wake up, `dequeue` and then handle that request
with the connfd it dequeued. at a `SIGTERM` the server will delete it's `Queue` and any other data structures 
it has allocated to the heap. The server will also do the same when it recieves the `SIGINT` signal. 


### Coherency and Atomicity
For this Project, I had to implement coherency and atomicity to our multi-threaded servers.
Luckily for us programmers, most of the heavy lifting has been done for us! Theses properties can
be implemented very simply using the function provided to us by the `files.h` file. More specifically
the `flock()` function which allows us to very easily let multiple threads read a file, while only letting 
one thread at a time write to a file when there are no readers. This function esentially solves our problem since 
when we use the `GET` request, we can grab the `shared` lock and so that lets other threads also using `GET` to
also operate on that file. When a `PUT` or `APPEND` is called, those functions grab the `exclusive` lock which means
that they're the only thread who is going to have access to this file. With these properties, implementing a `read-write` lock
is extremely simple. We do not have to worry too much about the coherency of the `LOG` file since the function `fprintf()`
that we use to write into the `LOG` file is inhernetly atomic. The locking mechanism for those writes are already built
in to `fprintf` so that's one less thing we as programmers have to think about. There is one special case that I needed
to keep track of for the method `PUT`. Esentially, when a put is executed it will try to open the `URI`, but if it doesn't exist
it will simply create the file. This is fine until you realize that in the process of creating that file, another thread can grab
the `file lock` before the PUT operation finishes. What we can do is simply aquire a lock before we start trying to open a file with `PUT`.
This is esentially all I have implemented for this section, this was a very simple design since most of the server was already written,
all I had to do was add more control to how the threads operate with `flock()` and another `mutex lock`. I actually did not have to change
`httpserver.c` at all, I only had to add thread synchronization inside of the `GET`, `PUT` and `APPEND` requests. Overall, this was a fairly
simple project to implement.



### Files
1. httpserver.c
2. io.c
3. io.h
4. List.c
5. List.h
6. parse.c
7. parse.h
8. node.c
9. node.h
10. request.c
11. request.h
12. messages.h (contains the all of the messages)
13. Makefile
14. queue.c
15. queue.h

