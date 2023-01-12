#pragma once

const char *err404 = "HTTP/1.1 404 Not Found\r\nContent-Length: 10\r\n\r\nNot Found\n";

const char *err403 = "HTTP/1.1 403 Forbidden\r\nContent-Length: 10\r\n\r\nForbidden\n";

const char *err500
    = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 22\r\n\r\nInternal Server Error\n";

const char *OK200 = "HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\nOK\n";

const char *OK201 = "HTTP/1.1 201 Created\r\nContent-Length: 8\r\n\r\nCreated\n";

const char *err400 = "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\nBad Request\n";

const char *err501 = "HTTP/1.1 501 Not Implemented\r\nContent-Length: 16\r\n\r\nNot Implemented\n";
