#pragma once

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

#include "node.h"
#include "List.h"

int cut_header(char *buf);

int cut_line(char *buf);

int process_request_line(char *request_line, char **method, char **URI, char **version);

int parse_request(char request[], int size_of_request, char *request_type, char *URI, char *version,
    Node **headers);
