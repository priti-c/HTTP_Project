/* HTTP Parse : Task 5: Handle Query Parameters
----------------------------------------
- Example request: /search?q=chatgpt
- Extract query string after '?'
- Print it on server side (e.g., Query=q=chatgpt).*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <time.h>

#define PORT 8080
#define SUPPORTED_METHOD "GET"
#define MAX_HOST_LEN 256
#define MAX_QUERY_LEN 256

ssize_t recv_all_headers(int fd, char *buf, size_t cap, int *recv_calls) {
    size_t used = 0;
    *recv_calls = 0;
    while (used + 1 < cap) {
        ssize_t n = recv(fd, buf + used, cap - used - 1, 0);
        (*recv_calls)++;
        if (n <= 0) {
            return n; 
        }
        used += (size_t)n;
        buf[used] = '\0';
        if (strstr(buf, "\r\n\r\n")) {
            break; 
        }
    }
    return (ssize_t)used;
}

void extract_host(char *req, char *host, size_t host_len) {
    char *host_hdr = strstr(req, "Host:");
    if (host_hdr) {
        host_hdr += 5; // Skip "Host:"
        while (*host_hdr == ' ') {
            host_hdr++;
        }
        char *end = strstr(host_hdr, "\r\n");
        if (end) {
            size_t len = end - host_hdr;
            if (len >= host_len) {
                len = host_len - 1;
            }
            strncpy(host, host_hdr, len);
            host[len] = '\0';
        } else {
            strncpy(host, host_hdr, host_len - 1);
            host[host_len - 1] = '\0';
        }
    } else {
        host[0] = '\0';
    }
}

void extract_query(char *path, char *query, size_t query_len) {
    char *query_start = strchr(path, '?');
    if (query_start) {
        query_start++; // Skip '?'
        strncpy(query, query_start, query_len - 1);
        query[query_len - 1] = '\0';
    } else {
        query[0] = '\0';
    }
}

void handle_request(int c, char *req) {
    char method[8], path[512], version[16];
    method[0] = path[0] = version[0] = '\0';
    sscanf(req, "%7s %511s %15s", method, path, version);

    printf("HTTP Method: %s\n", method);

    char host[MAX_HOST_LEN];
    extract_host(req, host, sizeof(host));
    printf("Host=%s\n", host);

    char query[MAX_QUERY_LEN];
    extract_query(path, query, sizeof(query));
    if (query[0] != '\0') {
        printf("Query=%s\n", query);
    }

    if (strcmp(method, SUPPORTED_METHOD) != 0) {
        const char *body = "Method Not Allowed\n";
        char hdr[200];
        snprintf(hdr, sizeof(hdr), "HTTP/1.1 405 Method Not Allowed\r\n"
                                   "Content-Type: text/plain\r\n"
                                   "Content-Length: %zu\r\n"
                                   "Connection: close\r\n\r\n", strlen(body));
        send(c, hdr, strlen(hdr), 0);
        send(c, body, strlen(body), 0);
    } else {
        char *body = NULL;
        char time_buf[64];
        if (strncmp(path, "/hello", 6) == 0) {
            body = "Hello Student!\n";
        } else if (strncmp(path, "/time", 5) == 0) {
            time_t now = time(NULL);
            struct tm *tm = localtime(&now);
            strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S\n", tm);
            body = time_buf;
        } else if (strncmp(path, "/search", 7) == 0) {
            body = "Search results...\n";
        } else {
            body = "Unknown Path!\n";
        }

        char hdr[200];
        snprintf(hdr, sizeof(hdr), "HTTP/1.1 200 OK\r\n"
                                   "Content-Type: text/plain\r\n"
                                   "Content-Length: %zu\r\n"
                                   "Connection: close\r\n\r\n", strlen(body));
        send(c, hdr, strlen(hdr), 0);
        send(c, body, strlen(body), 0);
    }
}

int main(void) {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("socket");
        exit(1);
    }

    int opt = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(s);
        exit(1);
    }

    struct sockaddr_in a = {0};
    a.sin_family = AF_INET;
    a.sin_addr.s_addr = INADDR_ANY;
    a.sin_port = htons(PORT);

    if (bind(s, (struct sockaddr *)&a, sizeof(a)) < 0) {
        perror("bind");
        close(s);
        exit(1);
    }

    if (listen(s, 16) < 0) {
        perror("listen");
        close(s);
        exit(1);
    }

    printf("Request parsing demo running on port %d\n", PORT);

    for (;;) {
        int c = accept(s, NULL, NULL);
        if (c < 0) {
            perror("accept");
            continue;
        }

        char req[8192];
        int recv_calls = 0;
        ssize_t n = recv_all_headers(c, req, sizeof(req), &recv_calls);
        if (n <= 0) {
            close(c);
            continue;
        }

        printf("Number of recv() calls: %d\n"recv() calls: %d\n", recv_calls);
        handle_request(c, req);
        close(c);
    }
}