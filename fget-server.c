#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>


#define SERVER_PORT 9999
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024
#define MAX_ARGS 3


int put(char *args[], int client_sockfd){

    char *path = args[2];

    printf("Operation: PUT\n");
    printf("Filename: %s\n", path);

    int fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        perror("File creation failed");
        return -1;
    }

    int bytes_written, bytes_recieved;
    char filebuffer[1024] = {0};
    while((bytes_recieved = recv(client_sockfd, filebuffer, BUFFER_SIZE, 0)) > 0) {
        bytes_written = write(fd, filebuffer, bytes_recieved);
        if (bytes_written < 0) {
            perror("Failed writting to file");
        }
    }

    close(fd);
    close(client_sockfd);

    return 0;

}


int main(){

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        return -1;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(SERVER_PORT);

    if (bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Error binding to port");
        return -1;
    }

    if(listen(sockfd, 1) < 0)
    {
        printf("Error while listening\n");
        return -1;
    }
    printf("\nListening for incoming connections.....\n");

    socklen_t client_size;
    struct sockaddr_in client_addr;
    int client_sockfd;
    client_size = sizeof(client_addr);
    client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_size);

    if (client_sockfd < 0)
    {
        printf("Can't accept\n");
        return -1;
    }

    printf("Client connected at IP: %s and port: %i\n",
           inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));

    char buffer[1024];
    int bytes_received = recv(client_sockfd, buffer, 1024, 0);
    buffer[bytes_received] = '\0';
    
    char *arg;
    char *rest = buffer;
    char *args[MAX_ARGS];
    int argn = 0;
    while ((arg = strtok_r(rest, " ", &rest))) {
        args[argn++] = arg;
        printf("Recieved argument: %s\n", arg);
    }

    if (strcmp(args[0], "PUT") == 0) {
        return put(args, client_sockfd);
    }
    
    close(client_sockfd);
    close(sockfd);

    return 0;
}