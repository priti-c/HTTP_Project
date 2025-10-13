// http_file_server.c : Task3: Map URL -> File
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 4096


void send_file(int client_fd, const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        // File not found → 404 response
        char not_found[] = "HTTP/1.1 404 Not Found\r\n"
                           "Content-Type: text/html\r\n\r\n"
                           "<html><body><h1>404 File Not Found</h1></body></html>";
        write(client_fd, not_found, strlen(not_found));
        return;
    }

    // Send HTTP response header
    char header[] = "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n\r\n";
    write(client_fd, header, strlen(header));

    // Send file contents
    char buf[BUF_SIZE];
    int n;
    while ((n = fread(buf, 1, BUF_SIZE, fp)) > 0) {
        write(client_fd, buf, n);
    }
    fclose(fp);
}

    // Parse request line and extract filename
    void parse_request(int client_fd, char *buffer) {
    char *ptr = strtok(buffer, " ");
    ptr = strtok(NULL, " ");
    char filename[100];
    strcpy(filename, ptr + 1); // Skip leading '/'

    if (strcmp(filename, "") == 0) {
        strcpy(filename, "index.html");
    }

    send_file(client_fd, filename);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    char buffer[BUF_SIZE];

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    //Assign address and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 5);

    printf("Server running on http://localhost:%d\n", PORT);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (client_fd < 0) continue;

        // Read request
        int valread = read(client_fd, buffer, BUF_SIZE - 1);
        buffer[valread] = '\0';
        printf("Request:\n%s\n", buffer);

	    send_file(client_fd,"index.html");
        close(client_fd);
    }

    // Modify main function to call parse_request
    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (client_fd < 0) continue;

        // Read request
        int valread = read(client_fd, buffer, BUF_SIZE - 1);
        buffer[valread] = '\0';
        printf("Request:\n%s\n", buffer);

        parse_request(client_fd, buffer);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
