// Day 4: Fork-based HTTP Server : Task2 : Keep child alive for multiple requests from same client.
#include <stdio.h>   //printf(),perror()
#include <stdlib.h>  //exit()
#include <string.h>  //strlen()
#include <unistd.h> //fork(),close(),write()
#include <arpa/inet.h> //sockaddr_in,inet_ntoa()
#include<time.h>   //time(),localtime(),strftime()

#define PORT 8080

//Function to handle each client request
    void handle_client(int client_fd) {
	    char buffer[1024];
	    ssize_t bytes_read;   //ssize_t : Used  for a count of bytes or an error indication. 
    while(1){
	    memset(buffer,0,sizeof(buffer));     //fill memory with a constant byte
    // void *memset(void *s, int c, size_t n);

//Read data from clients
	bytes_read = read(client_fd,buffer,sizeof(buffer) - 1);  //read from a file descriptor
  // ssize_t read(int fd, void *buf, size_t count);
  
	if(bytes_read <= 0){
		printf("Client is disconnected.\n");
		break;     //client closes connection
	}

//Remove trailing new line
 	buffer[strcspn(buffer, "\r\n")] = 0;
	
	printf("Received from client : %s\n",buffer);

//If client says "exit ", close the connection
	if(strcasecmp(buffer, "exit") == 0){
		printf("Client is requesting to close the connection.\n");
		break;
	}

//Prepare HTTP response	
    char response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
                      "<html><body><h1>Hello from Forked Server!</h1></body></html>";
    
//send response to client
    write(client_fd, response, strlen(response));
}    

//close client connection    
    close(client_fd);
}

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

//Handle the client's request          
            handle_client(client_fd);
            exit(0);     //End child process
        }

//Parent process closes client socket(handled by child)
        close(client_fd); 
    }

    close(server_fd);
    return 0;
}

