// Includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include "config.h"


#define BUFFER_SIZE 1024 // Constant buffer size


// Declare global variables
int port;
char *host;
char *usb1;
char *usb2; 


/**
 * Function name: help
 * Description: Print help page to stdout
 * Parameters: None
 * Returns: 0 (success), 1 (failure)
 **/
int help(){
    printf("--------\n");
    printf("  HELP\n");
    printf("--------\n");
    printf("Possible functions...\n");
    printf("[PUT] - fget PUT <local/file/path> <remote/file/path>\n");
    printf("[GET] - fget GET <remote/file/path>\n");
    printf("[MD] - fget MD <remote/folder/path>\n");
    printf("[RM] - fget RM <remote/file/path>\n");
    return 0;
}


/**
 * Function name: put
 * Description: Write a file to RAID storage
 * Parameters: A pointer to a character array of pointers which contain the clients arguments, integer argument count.
 * Returns: 0 (success), 1 (failure)
 **/
int put(char *argv[], int argc){

    char *localPath = argv[2];
    char *remotePath;
    if (argc == 3) {
        remotePath = argv[2];
    }
    else {
        remotePath = argv[3];
    }

    printf("Operation: PUT\n");
    printf("Local Path: %s\n", localPath);
    printf("Remote Path: %s\n", remotePath);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(host);
    server_address.sin_port = htons(port);

    int connect_result = connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address));

    if (connect_result < 0){
        printf("Failed to connect to server\n");
        return -1;
    }

    char arg_buffer[1024];
    if (argc == 3) {
        sprintf(arg_buffer, "%s %s %s", argv[1], argv[2], argv[2]);
    }
    else {
        sprintf(arg_buffer, "%s %s %s", argv[1], argv[2], argv[3]);
    }
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


/**
 * Function name: info
 * Description: Return information about a file (perms, owner, size, and access)
 * Parameters: A pointer to a character array of pointers which contain the clients arguments.
 * Returns: 0 (success), 1 (failure)
 **/
int info(char *argv[]){

    char *remotePath = argv[2];

    printf("Operation: INFO\n");
    printf("Remote Path: %s\n", remotePath);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(host);
    server_address.sin_port = htons(port);

    int connect_result = connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address));

    if (connect_result < 0){
        printf("Failed to connect to server\n");
        return -1;
    }

    char arg_buffer[1024];
    sprintf(arg_buffer, "%s %s", argv[1], argv[2]);
    send(sockfd, arg_buffer, strlen(arg_buffer), 0);

    char buffer[BUFFER_SIZE] = {0};
    int bytes_received = recv(sockfd, buffer, BUFFER_SIZE, 0);
    if (bytes_received < 0) {
        perror("Error receiving data from server");
        return -1;
    }

    printf("%s", buffer);

    return 0;

}


/**
 * Function name: md
 * Description: Create a directory in raid storage.
 * Parameters: A pointer to a character array of pointers which contain the clients arguments.
 * Returns: 0 (success), 1 (failure)
 **/
int md(char *argv[]){

    char *remotePath = argv[2];

    printf("Operation: MD\n");
    printf("Remote Path: %s\n", remotePath);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(host);
    server_address.sin_port = htons(port);

    int connect_result = connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address));

    if (connect_result < 0){
        printf("Failed to connect to server\n");
        return -1;
    }

    char arg_buffer[1024];
    sprintf(arg_buffer, "%s %s", argv[1], argv[2]);
    send(sockfd, arg_buffer, strlen(arg_buffer), 0);

    char buffer[BUFFER_SIZE] = {0};
    int bytes_received = recv(sockfd, buffer, BUFFER_SIZE, 0);
    if (bytes_received < 0) {
        perror("Error receiving data from server");
        return -1;
    }

    printf("%s", buffer);

    return 0;

}


/**
 * Function name: rm
 * Description: Remove a directory and all subcontents from RAID storage.
 * Parameters: A pointer to a character array of pointers which contain the clients arguments.
 * Returns: 0 (success), 1 (failure)
 **/
int rm(char *argv[]){

    char *remotePath = argv[2];

    printf("Operation: RM\n");
    printf("Remote Path: %s\n", remotePath);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(host);
    server_address.sin_port = htons(port);

    int connect_result = connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address));

    if (connect_result < 0){
        printf("Failed to connect to server\n");
        return -1;
    }

    char arg_buffer[1024];
    sprintf(arg_buffer, "%s %s", argv[1], argv[2]);
    send(sockfd, arg_buffer, strlen(arg_buffer), 0);

    char buffer[BUFFER_SIZE] = {0};
    int bytes_received = recv(sockfd, buffer, BUFFER_SIZE, 0);
    if (bytes_received < 0) {
        perror("Error receiving data from server");
        return -1;
    }

    printf("%s", buffer);

    return 0;

}


/**
 * Function name: get
 * Description: Return a file in RAID storage to client.
 * Parameters: A pointer to a character array of pointers which contain the clients arguments, integer argument count.
 * Returns: 0 (success), 1 (failure)
 **/
int get(char *argv[], int argc) {

    char *remotePath = argv[2];
    char *localPath;
    if (argc == 3) {
        localPath = argv[2];
    }
    else {
        localPath = argv[3];
    }

    printf("Operation: GET\n");
    printf("Remote Path: %s\n", remotePath);
    printf("Local Path: %s\n", localPath);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(host);
    server_address.sin_port = htons(port);

    int connect_result = connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address));

    if (connect_result < 0){
        printf("Failed to connect to server\n");
        return -1;
    }

    char arg_buffer[1024];
    sprintf(arg_buffer, "%s %s %s", argv[1], argv[2], argv[3]);
    send(sockfd, arg_buffer, strlen(arg_buffer), 0);

    int fd = open(localPath, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        perror("File creation failed");
        return -1;
    }

    int bytes_written, bytes_recieved;
    char filebuffer[1024] = {0};
    while((bytes_recieved = recv(sockfd, filebuffer, BUFFER_SIZE, 0)) > 0) {
        bytes_written = write(fd, filebuffer, bytes_recieved);
        if (bytes_written < 0) {
            perror("Failed writting to file");
        }
    }

    return 0;

}

int main(int argc, char *argv[]) {

    // Parse config file
    Config config;
    parse_config("config.ini", &config);

    // Print config values
    port = config.port;
    host = config.host;
    usb1 = config.usb1;
    usb2 = config.usb2;

    if (argc == 1) {
        help();
        return -1;
    }

    // Switch operation function on client argument value
    if (strcmp(argv[1], "PUT") == 0) {
        if ( (argc != 3) && (argc != 4) ) {
            printf("Invalid arguments\n");
            help();
            return -1;
        }
        return put(argv, argc);
    }
    else if (strcmp(argv[1], "GET") == 0) {
        if ( (argc != 3) && (argc != 4) ) {
            printf("Invalid arguments\n");
            help();
            return -1;
        }
        return get(argv, argc);
    }
    else if (strcmp(argv[1], "INFO") == 0) {
        if (argc != 3){
            printf("Invalid arguments\n");
            help();
            return -1;
        }
        return info(argv);
    }
    else if (strcmp(argv[1], "MD") == 0) {
        if (argc != 3){
            printf("Invalid arguments\n");
            help();
            return -1;
        }
        return md(argv);
    }
    else if (strcmp(argv[1], "RM") == 0) {
        if (argc != 3){
            printf("Invalid arguments\n");
            help();
            return -1;
        }
        return rm(argv);
    }
    else {
        printf("Invalid request type\n");
        help();
        return -1;
    }

    return 0;

}