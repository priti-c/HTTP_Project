//Basic HTTP server : Task2 :Change HTML content dynamically.
#include <stdio.h>      // For printf(), perror()[Input output function]
#include <stdlib.h>     // For exit() 
#include <string.h>     // For strlen(), strcmp()[String handling]
#include <unistd.h>     // For close(), read(), write()[]
#include <arpa/inet.h>  // For sockaddr_in, inet_ntoa()
#include <time.h>       // For time(), localtime(), strftime()

#define PORT 8080
#define BUFFER_SIZE 4096

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE] = {0};

    // Step 1: Create TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Step 2: Bind socket to address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Step 3: Listen for incoming connections
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server running on http://localhost:%d\n", PORT);

    // Step 4: Accept clients in loop
    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        // Step 5: Read and log HTTP request
        memset(buffer, 0, sizeof(buffer));
        read(client_fd, buffer, sizeof(buffer));
        printf("\n=== HTTP Request ===\n%s\n=====================\n", buffer);

        // Step 6: Parse HTTP method and path
        char method[16], path[1024];
        sscanf(buffer, "%s %s", method, path);

        // Step 7: Get current time
        time_t now = time(NULL);
        char time_str[64];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

        // Step 8: Get client IP address
        char *client_ip = inet_ntoa(client_addr.sin_addr);

        // Step 9: Append time & IP to HTML
        char content[2048];
        char body[1024];
        snprintf(content, sizeof(content),
                 "<html><body>%s<p>Time: %s</p><p>Client IP: %s</p></body></html>",
                 body, time_str, client_ip);

         // Step 10: Build full HTTP response
        char response[4096];
        snprintf(response, sizeof(response),
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/html\r\n"
                 "Content-Length: %ld\r\n"
                 "Connection: close\r\n\r\n%s",
                 strlen(content), content);

        // Step 11: Send response & close connection
        write(client_fd, response, strlen(response));
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
