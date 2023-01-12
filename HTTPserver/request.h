#pragma once

#include "node.h"

#define BLOCK 4096

int GET(char *URI, int connfd);

int PUT(char *URI, int connfd, int size_of_content, char *request_body, int size_of_read_body);

int APPEND(char *URI, int connfd, int size_of_content, char *request_body, int size_of_read_body);

//                                      NOTE:
// we have (int size_of_read_body) beacause we cannot find the size in the function!
// we are passing in a pointer offset so we do not technically know how much of the
// message body we've read in into the initial header buffer. we need to pass that into the function
// to avoid writing bytes outside of our buffer.
