#include <sys/stat.h>
#include <sys/file.h>
#include <pthread.h> // for locks!
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "request.h"
#include "io.h"
#include "List.h"
#include "node.h"

#define BLOCK 4096

pthread_mutex_t put_lock = PTHREAD_MUTEX_INITIALIZER;

int GET(char *URI, int connfd) { // Inspired by Professor Q's demo!
    int fd = open(URI, O_RDONLY); // open URI!
    if (fd == -1) {
        if (errno == 2) {
            char *returnString = "HTTP/1.1 404 Not Found\r\nContent-Length: 10\r\n\r\nNot Found\n";
            write_bytes(connfd, (uint8_t *) returnString, strlen(returnString));
            return 404;
        } else if (errno == 13 || errno == 21) {
            char *returnString = "HTTP/1.1 403 Forbidden\r\nContent-Length: 10\r\n\r\nForbidden\n";
            write_bytes(connfd, (uint8_t *) returnString, strlen(returnString));
            return 403;
        }
        // need to check for directory
    }
    // we know that this file exists and we can read from it so we're
    // gonna grab a shared lock because we can have multiple readers
    flock(fd, LOCK_SH); // grab a shared lock!
    struct stat size;
    fstat(fd, &size);
    uint64_t file_size = size.st_size;
    if (!(S_ISREG(size.st_mode))) { // file is a directory
        char *returnString = "HTTP/1.1 403 Forbidden\r\nContent-Length: 10\r\n\r\nForbidden\n";
        write_bytes(connfd, (uint8_t *) returnString, strlen(returnString));
        return 403;
    }
    char file_size_string[32];
    snprintf(file_size_string, 32, "%lu", file_size); // we need to turn the file size to a string
    // max number for an unsigned 64 bit integer is 20 chars long!
    char ok_response[2048] = "HTTP/1.1 200 OK\r\nContent-Length: "; // starting message for success!
    strncat(ok_response, file_size_string, 32); // add size of file to ok_response
    strncat(ok_response, "\r\n\r\n", 4);
    write_bytes(connfd, (uint8_t *) ok_response, strlen(ok_response));
    uint8_t buf[BLOCK];
    int bytes;
    while ((bytes = read(fd, buf, BLOCK)) > 0) {
        if (bytes == -1) {
            close(fd);
            char *err_response
                = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 22\r\n\r\nInternal Server "
                  "Error\n";
            write_bytes(connfd, (uint8_t *) err_response, strlen(err_response));
            return 500;
        }
        ssize_t bytes_written = 0, curr_write = 0;
        while ((curr_write = write(connfd, buf + bytes_written, bytes - bytes_written)) > 0) {
            bytes_written += curr_write;
            if (bytes_written >= bytes) {
                break;
            }
        }
    }
    close(fd);
    return 200;
}

int PUT(char *URI, int connfd, int size_of_content, char *request_body, int size_of_read_body) {
    //                 NOTE:
    // in order to use this function correctly,
    // you must take the message out of the full request
    // put it in another string and then pass it into this function.
    // this function will make no attempt to find a header ending,
    // it will just start writing bytes you give it!
    // if the header was 2048 bytes, pass in an empty string!

    int status = 200;
    int bytes_written = 0, bytes_read = 0;
    uint8_t buf[BLOCK]; // for reading bytes from the URI if needed
    pthread_mutex_lock(&put_lock); // we need to lock just in case another thread tries to put!
    int fd = open(URI, O_WRONLY | O_TRUNC); // open URI!
    if (fd == -1) {
        if (errno == 13 || errno == 21) { // not enough permission or directory
            char *returnString = "HTTP/1.1 403 Forbidden\r\nContent-Length: 10\r\n\r\nForbidden\n";
            write_bytes(connfd, (uint8_t *) returnString, strlen(returnString));
            pthread_mutex_unlock(&put_lock); // put failed I don't need to lock anymore
            return 403;
        } else { // file didn't exist!
            fd = open(URI, O_WRONLY | O_CREAT);
            status = 201;
        }
    }
    flock(fd, LOCK_EX);
    pthread_mutex_unlock(&put_lock);
    if (size_of_read_body <= size_of_content) {
        bytes_written = write_bytes(fd, (uint8_t *) request_body, size_of_read_body);
    } else {
        bytes_written = write_bytes(fd, (uint8_t *) request_body, size_of_content);
    }
    // ^ this is for the message read in with the header ^
    while (bytes_written < size_of_content) {
        if ((size_of_content - bytes_written) < BLOCK) {
            bytes_read = read(connfd, buf, size_of_content - bytes_written);
        } else {
            bytes_read = read(connfd, buf, BLOCK);
        }
        bytes_written += write_bytes(fd, buf, bytes_read);
        if (bytes_read == -1 || bytes_written == -1) {
            status = 500; // something strange happened abort!
            break;
        }
    }
    // we now need to construct our response to the client
    close(fd); // make sure you close the file!
    if (status == 200) {
        char *ok_response = "HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\nOK\n";
        write_bytes(connfd, (uint8_t *) ok_response, strlen(ok_response));
        return 200;
    } else if (status == 201) {
        char *ok_response = "HTTP/1.1 201 Created\r\nContent-Length: 8\r\n\r\nCreated\n";
        write_bytes(connfd, (uint8_t *) ok_response, strlen(ok_response));
        return 201;
    } else { // something very strange happened! (we ded.)
        char *err_response
            = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 22\r\n\r\nInternal Server "
              "Error\n";
        write_bytes(connfd, (uint8_t *) err_response, strlen(err_response));
        return 500;
    }
    return -1; // something REALLY weird happened!
}

int APPEND(char *URI, int connfd, int size_of_content, char *request_body, int size_of_read_body) {
    //                 NOTE:
    // in order to use this function correctly,
    // you must take the message out of the full request
    // put it in another string and then pass it into this function.
    // this function will make no attempt to find a header ending,
    // it will just start writing bytes you give it!
    // if the header was 2048 bytes, pass in an empty string!

    int status = 200;
    int bytes_written = 0, bytes_read = 0;
    uint8_t buf[BLOCK]; // for reading bytes from the URI if needed
    int fd = open(URI, O_WRONLY | O_APPEND); // open URI!
    if (fd == -1) {
        if (errno == 13 || errno == 21) { // not enough permission
            char *returnString = "HTTP/1.1 403 Forbidden\r\nContent-Length: 10\r\n\r\nForbidden\n";
            write_bytes(connfd, (uint8_t *) returnString, strlen(returnString));
            return 403;
        } else if (errno == 2) { // file didn't exist!
            char *returnString = "HTTP/1.1 404 Not Found\r\nContent-Length: 10\r\n\r\nNot Found\n";
            write_bytes(connfd, (uint8_t *) returnString, strlen(returnString));
            return 404;
        } else { // something else I can't explain!
            char *err_response
                = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 22\r\n\r\nInternal Server "
                  "Error\n";
            write_bytes(connfd, (uint8_t *) err_response, strlen(err_response));
            return 500;
        }
    }
    flock(fd, LOCK_EX);
    if (size_of_read_body <= size_of_content) {
        bytes_written = write_bytes(fd, (uint8_t *) request_body, size_of_read_body);
    } else {
        bytes_written = write_bytes(fd, (uint8_t *) request_body, size_of_content);
    }
    // ^ this is for the message read in with the header ^
    while (bytes_written < size_of_content) {
        bytes_read = read_bytes(connfd, buf, BLOCK);
        bytes_written += write_bytes(fd, buf, bytes_read);
        if (bytes_read == -1 || bytes_written == -1) {
            status = 500; // something strange happened abort!
            break;
        }
    }
    // we now need to construct our response to the client
    close(fd); // make sure you close the file!
    if (status == 200) {
        char *ok_response = "HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\nOK\n";
        write_bytes(connfd, (uint8_t *) ok_response, strlen(ok_response));
        return 200;
    } else { // something very strange happened! (we ded.)
        char *err_response
            = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 22\r\n\r\nInternal Server "
              "Error\n";
        write_bytes(connfd, (uint8_t *) err_response, strlen(err_response));
        return 500;
    }
    return -1; // something REALLY weird happened!
}
