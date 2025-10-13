// http_file_server.c : Task1 : Serve index.html
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 4096

void send_index(int client_fd){
	FILE *fp =fopen("index.html","rb");
	if(!fp){
		char error[] = "HTTP/1.1 500 Internal server error\r\n\r\n";
		write(client_fd,error,strlen(error));
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

	send_index(client_fd);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
