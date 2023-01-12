#include "parse.h"

#define VERSION      "(HTTP/...)"
#define URI_REGEX    "(/[a-zA-Z._0-9]{0,15})"
#define METHOD       "([a-zA-Z]{1,8})"
#define REQUEST_LINE "^" METHOD " " URI_REGEX " " VERSION "\r\n"
#define KEY          "([^\r\n\t ]+)"
#define VAL          "(.+)"
#define HEADER_LINE  "^" KEY ": " VAL "\r\n"

int cut_header(char *buf) { // returns index of the last byte of the header
    int index = 0;
    int sequence = 0;
    while (index < 2048) {
        if (buf[index] == '\r') {
            if (sequence == 0) {
                sequence++;
            }
            if (sequence == 2) {
                sequence++;
            }

        } else if (buf[index] == '\n') {
            if (sequence == 1) {
                sequence++;
            }
            if (sequence == 3) {
                sequence++;
            }
        } else {
            sequence = 0; // not \r or \n restart sequence
        }

        if (sequence == 4) {
            return index;
        }
        index++;
    }
    return -1;
}

int cut_line(char *buf) { // returns the index of the end of the current line!
    //                              NOTE:
    //   when dealing with multiple lines, please pass in the index offset.
    int index = 0;
    int sequence = 0;
    while (index < 2048) {
        if (buf[index] == '\r') {
            if (sequence == 0) {
                sequence++;
            }

        } else if (buf[index] == '\n') {
            if (sequence == 1) {
                sequence++;
            }
        } else {
            sequence = 0; // not \r or \n restart sequence
        }
        if (sequence == 2) {
            return index;
        }
        index++;
    }
    return -1;
}

int process_request_line(char *request_line, char **method, char **URI, char **version) {
    regmatch_t match[4];
    int indicies[2];
    regex_t regex;
    if (regcomp(&regex, REQUEST_LINE, REG_EXTENDED)) {
        printf("could not compile regex!\n");
        return 1;
    }
    int matches = regexec(&regex, request_line, 4, match, 0);
    regfree(&regex);
    if (matches == 0) { // we found a match!
        indicies[0] = match[1].rm_so;
        indicies[1] = match[1].rm_eo;
        snprintf(*method, (indicies[1] - indicies[0]) + 1, "%s", request_line + indicies[0]);
        indicies[0] = match[2].rm_so;
        indicies[1] = match[2].rm_eo;
        snprintf(*URI, (indicies[1] - indicies[0]), "%s", request_line + indicies[0] + 1);
        indicies[0] = match[3].rm_so;
        indicies[1] = match[3].rm_eo;
        snprintf(*version, (indicies[1] - indicies[0]) + 1, "%s", request_line + indicies[0]);
    } else {
        return 1;
    }
    return 0;
}

int process_header_line(char *header_line, char *key, char *val) {
    regmatch_t match[3];
    int indicies[2];
    regex_t regex;
    if (regcomp(&regex, HEADER_LINE, REG_EXTENDED)) {
        printf("could not compile regex!\n");
        return 1;
    }
    int matches = regexec(&regex, header_line, 4, match, 0);
    regfree(&regex);
    if (matches == 0) { // we found a match!
        indicies[0] = match[1].rm_so;
        indicies[1] = match[1].rm_eo;
        snprintf(key, (indicies[1] - indicies[0]) + 1, "%s", header_line + indicies[0]);
        indicies[0] = match[2].rm_so;
        indicies[1] = match[2].rm_eo;
        snprintf(val, (indicies[1] - indicies[0]) + 1, "%s", header_line + indicies[0]);
    } else {
        return 1;
    }
    return 0;
}

int parse_request(char request[], int size_of_request, char *request_type, char *URI, char *version,
    Node **headers) {
    int cursor = 0;
    int cursor_end = 0;
    cursor_end = cut_line(request);
    if (cursor_end == -1) {
        return 400; // bad request couldn't find end of line
    }
    char *request_line = strndup(request, cursor_end + 1); // makes a copy of the request line;
    int request_status = process_request_line(request_line, &request_type, &URI, &version);
    if (request_status != 0 || strcmp(version, "HTTP/1.1")) {
        return 400;
    }

    // this sscanf needs work! need to have stricter formatting!
    free(request_line); // free the mem that strndup allocates

    char *header_line;
    char key[32];
    char val[32];
    int header_status;
    while (cursor_end < (size_of_request - 3)) { // once the cursor_end hits the last \r\n\r\n stop
        cursor = cursor_end + 1;
        cursor_end = cut_line(request + cursor) + cursor;
        header_line
            = strndup(request + cursor, (cursor_end - cursor) + 1); // takes out header line!
        header_status = process_header_line(header_line, key, val);
        free(header_line);
        //printf("%s: %s\n", key, val);
        if (header_status != 0) {
            return 400;
        }
        *headers = list_insert(*headers, key, val); // add to requests!
    }
    return 200;
}
