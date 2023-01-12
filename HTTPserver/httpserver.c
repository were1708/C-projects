#include <err.h>
#include <fcntl.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <limits.h>

#include "io.h"
#include "List.h"
#include "node.h"
#include "messages.h"
#include "request.h"
#include "parse.h"
#include "queue.h"

#define OPTIONS              "t:l:"
#define BUF_SIZE             4096
#define HEADER_BLOCK         2048
#define DEFAULT_THREAD_COUNT 4
#define GET_REQUEST          44
#define PUT_REQUEST          45
#define APPEND_REQUEST       46
#define LOG(...)             fprintf(logfile, __VA_ARGS__);

// SHARED STATE
static FILE *logfile;
pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty_queue = PTHREAD_COND_INITIALIZER;
pthread_cond_t full_queue = PTHREAD_COND_INITIALIZER;
queue *request_queue; // queue for connfds
static int threads = DEFAULT_THREAD_COUNT;
pthread_t *arr_threads;

// Converts a string to an 16 bits unsigned integer.
// Returns 0 if the string is malformed or out of the range.
static size_t strtouint16(char number[]) {
    char *last;
    long num = strtol(number, &last, 10);
    if (num <= 0 || num > UINT16_MAX || *last != '\0') {
        return 0;
    }
    return num;
}

// Creates a socket for listening for connections.
// Closes the program and prints an error message on error.
static int create_listen_socket(uint16_t port) {
    struct sockaddr_in addr;
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        err(EXIT_FAILURE, "socket error");
    }
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htons(INADDR_ANY);
    addr.sin_port = htons(port);
    if (bind(listenfd, (struct sockaddr *) &addr, sizeof addr) < 0) {
        err(EXIT_FAILURE, "bind error");
    }
    if (listen(listenfd, 128) < 0) {
        err(EXIT_FAILURE, "listen error");
    }
    return listenfd;
}

static void handle_connection(int connfd) {
    // make the compiler not complain
    (void) connfd;
    uint8_t header_buffer[HEADER_BLOCK] = { 0 };

    Node *headers = list_create(); // allocate mem for linked list
    char request_type[8] = { 0 };
    char URI[19] = { 0 };
    char version[9] = { 0 };
    int size_of_content = -1;

    // int header_size = recv(connfd, header_buffer, HEADER_BLOCK, MSG_WAITALL);
    int header_size = 0;
    int bytes_read = 0;
    do {
        int bytes_read = read(connfd, header_buffer + header_size, HEADER_BLOCK - header_size);
        header_size += bytes_read;
    } while (cut_header((char *) header_buffer) == -1 || bytes_read != 0);

    int index = cut_header((char *) header_buffer);
    if (index == -1) {
        write_bytes(connfd, (uint8_t *) err400, strlen(err400));
        return;
    }

    char cpyheader[2048] = { 0 };
    memcpy(cpyheader, header_buffer, index + 1);
    // char request_type[8] = { 0 };
    // char URI[19] = { 0 };
    // char version[9] = { 0 };
    // int size_of_content = -1;

    int code = parse_request(cpyheader, (index + 1), request_type, URI, version, &headers);
    if (code == 400) {
        write_bytes(connfd, (uint8_t *) err400, strlen(err400));
        return;
    }
    int request_num;
    if (!(strcmp(request_type, "GET")) || !(strcmp(request_type, "get"))) { // request is get!
        request_num = GET_REQUEST;
    } else if (!(strcmp(request_type, "PUT"))
               || !(strcmp(request_type, "put"))) { // request is put!
        request_num = PUT_REQUEST;
    } else if (!(strcmp(request_type, "APPEND"))
               || !(strcmp(request_type, "append"))) { // request is APPEND
        request_num = APPEND_REQUEST;
    } else { // not an implemented request!
        write_bytes(connfd, (uint8_t *) err501, strlen(err501));
        list_delete(&headers); // delete linked list for mem leaks!
        return;
    }
    Node *length = list_find(headers, "Content-Length");
    if (length) {
        size_of_content = strtol(length->val, NULL, 10);
    }
    if (size_of_content == -1 || errno == EINVAL) { // if no Content-Length!
        if (request_num != GET_REQUEST) {
            write_bytes(connfd, (uint8_t *) err400, strlen(err400));
            return;
        }
    }

    int status; // for logging the status number
    long request_id_number = 0; // for the request ID
    Node *Request_ID = list_find(headers, "Request-Id");
    if (Request_ID) {
        request_id_number = strtol(Request_ID->val, NULL, 10);
    }

    if (request_num == GET_REQUEST) {
        status = GET(URI, connfd);
        LOG("%s,/%s,%d,%ld\n", request_type, URI, status, request_id_number);
        fflush(logfile);
    } else if (request_num == PUT_REQUEST) {
        status = PUT(URI, connfd, size_of_content, (char *) header_buffer + (index + 1),
            header_size - (index + 1));
        // we pass the header buffer but start the pointer at where the body starts!
        LOG("%s,/%s,%d,%ld\n", request_type, URI, status, request_id_number);
        fflush(logfile);
    } else if (request_num == APPEND_REQUEST) {
        status = APPEND(URI, connfd, size_of_content, (char *) header_buffer + (index + 1),
            header_size - (index + 1));
        // we pass the header buffer but start the pointer at where the body starts!
        LOG("%s,/%s,%d,%ld\n", request_type, URI, status, request_id_number);
        fflush(logfile);

    } else {
        write_bytes(connfd, (uint8_t *) err501, strlen(err501));
        status = 501;
        LOG("%s,/%s,%d,%ld\n", request_type, URI, status, request_id_number);
        fflush(logfile);
    }
    list_delete(&headers);
    return;
}

void *thread_function(void *arg) {
    (void) arg;
    while (1) {
        pthread_mutex_lock(&queue_lock);
        while (empty(request_queue)) {
            pthread_cond_wait(&full_queue, &queue_lock); // wait on "full" queue
        }
        int connfd = dequeue(request_queue);
        pthread_cond_signal(&empty_queue);
        pthread_mutex_unlock(&queue_lock);
        if (connfd != INT_MIN) {
            handle_connection(connfd);
            close(connfd);
        }
    }
    return NULL;
}

static void sigterm_handler(int sig) {
    if (sig == SIGTERM) {
        warnx("received SIGTERM");
        free(arr_threads);
        delete_queue(&request_queue);
        fclose(logfile);
        exit(EXIT_SUCCESS);
    }
}

static void sigint_handler(int sig) {
    if (sig == SIGINT) {
        warnx("received SIGTERM");
        free(arr_threads);
        delete_queue(&request_queue);
        fclose(logfile);
        exit(EXIT_SUCCESS);
    }
}

static void usage(char *exec) {
    fprintf(stderr, "usage: %s [-t threads] [-l logfile] <port>\n", exec);
}

int main(int argc, char *argv[]) {
    int opt = 0;
    logfile = stderr;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 't':
            threads = strtol(optarg, NULL, 10);
            if (threads <= 0) {
                errx(EXIT_FAILURE, "bad number of threads");
            }
            break;
        case 'l':
            logfile = fopen(optarg, "w");
            if (!logfile) {
                errx(EXIT_FAILURE, "bad logfile");
            }
            break;
        default: usage(argv[0]); return EXIT_FAILURE;
        }
    }

    if (optind >= argc) {
        warnx("wrong number of arguments");
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    uint16_t port = strtouint16(argv[optind]);
    if (port == 0) {
        errx(EXIT_FAILURE, "bad port number: %s", argv[1]);
    }

    signal(SIGPIPE, SIG_IGN);
    signal(SIGTERM, sigterm_handler);
    signal(SIGINT, sigint_handler);

    arr_threads = malloc(sizeof(pthread_t) * threads);
    request_queue = create_queue(2048); // request queue will be 2048 elements long
    for (int i = 0; i < threads; i++) {
        int check = pthread_create(&arr_threads[i], NULL, thread_function, NULL);
        if (check != 0) { // thread was not creating!
            errx(EXIT_FAILURE, "Thread start failed");
        }
    }

    int listenfd = create_listen_socket(port);

    for (;;) {
        int connfd = accept(listenfd, NULL, NULL);
        if (connfd < 0) {
            warn("accept error");
            continue;
        }
        pthread_mutex_lock(&queue_lock);
        while (full(request_queue)) {
            pthread_cond_wait(&empty_queue, &queue_lock); // waiting on "empty" queue
        }
        enqueue(request_queue, connfd);
        pthread_cond_signal(&full_queue); // signal "full" queue
        pthread_mutex_unlock(&queue_lock);
    }

    return EXIT_SUCCESS;
}
