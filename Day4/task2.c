// TCP Echo Server - Task 2: Send welcome message immediately after connection
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define PORT 8080
#define BUF_SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUF_SIZE];
    socklen_t addrlen = sizeof(client_addr);

    // Create TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d\n", PORT);

    // Accept client connection
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addrlen);
    if (client_fd < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Show client information
    printf("Client connected: %s:%d\n",
           inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));

    // Send welcome message
    char *welcome_message = "Welcome to the TCP Echo Server!!!\n";
    write(client_fd, welcome_message, strlen(welcome_message));

    // Read messages from client and echo back
    while (1) {
        memset(buffer, 0, BUF_SIZE);
        int bytes = read(client_fd, buffer, sizeof(buffer) - 1);

        if (bytes <= 0) {
            printf("Client disconnected.\n");
            break;
        }

        // Print client message
        printf("Client says: %s\n", buffer);

        // Echo back
        write(client_fd, buffer, bytes);
    }

    // Close sockets
    close(client_fd);
    close(server_fd);

    return 0;
}
