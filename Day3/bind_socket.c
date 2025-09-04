#include <stdio.h>    // For printf(), perror()[Input output function]
#include <stdlib.h>  // For exit() 
#include <string.h>  // For strlen(), strcmp()[String handling]
#include <unistd.h>   // For close(), read(), write()[Unix System calls]
#include <netinet/in.h> 
 
#define PORT 8080    // Port number to use 
#define BUFFER_SIZE 1024   // Size of buffer to reading data

int main() {
    int server_fd;	// File descriptor for the server socket 
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
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Bind Sucessful to port 8080 !!!\n");

    // 3. Close sockets
    close(server_fd);

    return 0;
}
