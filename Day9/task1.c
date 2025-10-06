// Day 4: Fork-based HTTP Server : Task1 : Show process details for each client.
#include <stdio.h>   //printf(),perror()
#include <stdlib.h>  //exit()
#include <string.h>  //strlen()
#include <unistd.h> //fork(),close(),write()
#include <arpa/inet.h> //sockaddr_in,inet_ntoa()
#include<time.h>   //time(),localtime(),strftime()

#define PORT 8080

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addrlen = sizeof(client_addr);

//Create TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1){
      perror("Socket creation failed");
      exit(EXIT_FAILURE);
    }

//Allow port reuse
    int option = 1;
    setsockopt(server_fd,SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    
//Bind socket to IP and port 
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
      perror("Bind Failed");
      close(server_fd);
      exit(EXIT_FAILURE);
    }

//Listen for incoming connections
    if(listen(server_fd, 10) < 0){
      perror("Listen Failed");
      close(server_fd);
      exit(EXIT_FAILURE);
     }
    
    printf("Fork based HTTP server is running on port %d\n",PORT);

//Accept clients in loop
    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addrlen);
    if(client_fd < 0){
	 perror("Accept Failed");
	 continue;
	}

//Fork new process for each client
     pid_t pid = fork();
     if (fork() == 0) {       //child process
	close(server_fd);   //child doesn't need server socket

//Get current time (connection started)
     time_t now = time(NULL);
     char time_str[64];
     strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

//Get client IP and port
     char *client_ip = inet_ntoa(client_addr.sin_addr);
     int client_port = ntohs(client_addr.sin_port);

//Print process and connection details
    printf("\n----New Client Connection---\n");
    printf("Child PID: %d\n", getpid());
    printf("Client IP: %s\n", client_ip);
    printf("Client Port: %d\n", client_port);
    printf("Connection Time: %s\n",time_str);

    printf("-----------------------\n");
    }

//Parent process closes client socket(handled by child)
        close(client_fd); 
    }

    close(server_fd);
    return 0;
}
