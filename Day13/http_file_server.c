// http_file_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 4096


    // Modify send_file function to support different file types
    void send_file(int client_fd, const char *filename) {
        FILE *fp = fopen(filename, "rb");
        if (!fp) {
            char not_found[] = "HTTP/1.1 404 Not Found\r\n"
                            "Content-Type: text/html\r\n\r\n"
                            "<html><body><h1>404 File Not Found</h1></body></html>";
            write(client_fd, not_found, strlen(not_found));
            return;
        }

        char *ext = strrchr(filename, '.');
        char *content_type = "text/plain";
        if (strcmp(ext, ".html") == 0) {
            content_type = "text/html";
        } else if (strcmp(ext, ".jpg") == 0) {
            content_type = "image/jpeg";
        } else if (strcmp(ext, ".png") == 0) {
            content_type = "image/png";
        }

        char header[100];
        sprintf(header, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", content_type);
        write(client_fd, header, strlen(header));

        char buf[BUF_SIZE];
        int n;
        while ((n = fread(buf, 1, BUF_SIZE, fp)) > 0) {
            write(client_fd, buf, n);
        }
        fclose(fp);
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
  
    close(client_fd);
    close(server_fd);
    return 0;
}
