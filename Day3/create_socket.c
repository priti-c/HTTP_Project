#include <stdio.h>    // For printf(), perror()[ Standard Input output function]
#include <stdlib.h>   // For exit() 
#include <string.h>  // For strlen(), strcmp()[Function for String handling]
#include <unistd.h>  // For close(), read(), write()[Unix System calls]  
#include <netinet/in.h>  // For struct sockaddr_in(),htons(),INADDR_ANY()

#define PORT 8080     //  Common port number to use for testing wev servers 
#define BUFFER_SIZE 1024   // Size of buffer to reading data temporarily for data sent/received (1 KB)

int main() {
    int server_fd;     // File descriptor for the server socket 
    struct sockaddr_in address;   //Strutures that stores server address information
    char buffer[BUFFER_SIZE] = {0};  // A buffer intialized with 0's used to store client data
    int addrlen = sizeof(address);     //Size of address struture (needed for accept())
    
    // 1. Create socket file descriptor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);  //socket() creates a socket
 //   int socket(int domain,int type,int protocol)
   
    if (server_fd < 0) {
        perror("Socket Failed");    //If socket creation failed,print error message.
        exit(EXIT_FAILURE);
    }
    
    printf("Socket Creation Done !!!\n");    // If socket creation success, print done message.
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