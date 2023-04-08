#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define SERVER_PORT 9999
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024


int help(){
    return 0;
}

int put(char *argv[]){

    char *localPath = argv[2];
    char *remotePath = argv[3];

    printf("Operation: PUT\n");
    printf("Local Path: %s\n", localPath);
    printf("Remote Path: %s\n", localPath);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_address.sin_port = htons(SERVER_PORT);

    int connect_result = connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address));

    if (connect_result < 0){
        printf("Failed to connect to server\n");
        return -1;
    }

    char arg_buffer[1024];
    sprintf(arg_buffer, "%s %s %s", argv[1], argv[2], argv[3]);
    send(sockfd, arg_buffer, strlen(arg_buffer), 0);

    FILE *file = fopen(localPath, "rb");
    if (!file){
        perror("Local file does not exist");
        return -1;
    }

    fseek(file, 0L, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    char* buffer = malloc(file_size);
    if (!buffer){
        perror("Failed to allocate memory for reading file");
        return -1;
    }

    size_t bytes_read = fread(buffer, 1, file_size, file);
    if (bytes_read != file_size){
        perror("Failed to read local file");
        return -1;
    }

    fclose(file);

    size_t total_sent = 0;
    while (total_sent < file_size){
        size_t remaining = file_size - total_sent;
        size_t chunk_size = remaining > BUFFER_SIZE ? BUFFER_SIZE : remaining;
        ssize_t sent = send(sockfd, buffer + total_sent, chunk_size, 0);
        if (sent == -1){
            perror("Failed to send buffer chunk");
            return -1;
        }
        total_sent += sent;
    }

    free(buffer);

    close(sockfd);

    return 0;
}

int main(int argc, char *argv[]) {

    if (argc != 4){
        printf("Invalid arguments\n");
        help();
        return -1;
    }

    if (strcmp(argv[1], "PUT") == 0) {
        return put(argv);
    }
    else {
        printf("Invalid request type\n");
        help();
        return -1;
    }

}