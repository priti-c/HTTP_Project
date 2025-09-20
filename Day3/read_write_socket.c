#include <stdio.h>    // For printf(), perror()[Input output function]
#include <stdlib.h>  // For exit() 
#include <string.h>  // For strlen(), strcmp()[String handling]
#include <unistd.h>   // For close(), read(), write()[Unix System calls]
#include <netinet/in.h> 
 
#define PORT 8080    // Port number to use 
#define BUFFER_SIZE 1024   // Size of buffer to reading data

int main() {
    int server_fd,client_fd;	// File descriptor for the server socket 
    struct sockaddr_in address;  //Strutures that stores server address information
    char buffer[BUFFER_SIZE] = {0};  // A buffer intialized with 0's used to store client data
    int addrlen = sizeof(address);   //Size of address struture (needed for accept())
    
    // 1. Create socket file descriptor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);   //socket() creates a socket
 //   int socket(int domain,int type,int protocol)
   
    if (server_fd < 0) {
        perror("socket failed");   //If socket creation failed,print error message.
        exit(EXIT_FAILURE);
    }
    printf("Socket Creation Done\n");  // If socket creation success, print done message.

    // 2. Bind the socket to IP/Port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // 0.0.0.0
    address.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");    //If socket binding failed,print error message.
        exit(EXIT_FAILURE);
    }
    printf("Bind Sucessful to port 8080 !!!\n");

    // 3. Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");  //If socket listen failed,print error message.
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d...\n", PORT);

    // 4. Accept a connection
    client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (client_fd< 0) {
        perror("accept failed");    //If socket accept failed,print error message.
        exit(EXIT_FAILURE);
    }
    printf("Server connection is sucessful to port 8080 !!!\n");

    // 5. Read from client
    read(client_fd, buffer, BUFFER_SIZE);
    printf("Client says: %s\n", buffer);

    // 6. Write response to client
    char *response = "Hello from server!\n";
    write(client_fd, response, strlen(response));

    // 7. Close sockets
    close(client_fd);
    close(server_fd);

    return 0;
}

/*
 * It successfully creates a TCP socket (a communication endpoint).
 * If creation fails, it shows an error and exits.
 * It does not bind, listen, or accept connections yet, so it can’t talk to clients.
 
  ## int socket(int domain, int type, int protocol);
      AF_INET → Use IPv4.
      SOCK_STREAM → TCP connection (reliable, connection-oriented).
      0 → Default protocol (TCP for SOCK_STREAM).
*/