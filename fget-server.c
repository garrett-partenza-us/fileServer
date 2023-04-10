#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include "config.h"
#include <pthread.h>


#define SERVER_PORT 9999
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024
#define MAX_ARGS 3


int port;
char *host;
char *usb1;
char *usb2;
pthread_mutex_t lock;


int put(char *args[], int client_sockfd){

    char *path = args[2];

    printf("Operation: PUT\n");
    printf("Filename: %s\n", path);

    char path1[strlen(usb1) + strlen(path) + 1];
    char path2[strlen(usb2) + strlen(path) + 1];

    strcpy(path1, usb1);
    strcat(path1, path);
    strcpy(path2, usb2);
    strcat(path2, path);

    int fd1 = open(path1, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd1 < 0) {
        printf("File creation failed for usb1");
    }

    int fd2 = open(path2, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd2 < 0) {
        printf("File creation failed for usb2");
    }

    if ( (fd1 < 0) && (fd2 < 0) ) {
        char buffer[BUFFER_SIZE] = {0};
        strcat(buffer, "File server currently unavailable\n");
        if (send(client_sockfd, buffer, strlen(buffer), 0) < 0) {
            perror("Error sending server status message");
        }
        return -1;
    }

    int bytes_written1, bytes_written2, bytes_recieved;
    char filebuffer[1024] = {0};
    while((bytes_recieved = recv(client_sockfd, filebuffer, BUFFER_SIZE, 0)) > 0) {
        if (fd1) {
            bytes_written1 = write(fd1, filebuffer, bytes_recieved);
            if (bytes_written1 < 0) {
                perror("Failed writting to usb1");
            }
        }
        if (fd2) {
            bytes_written2 = write(fd2, filebuffer, bytes_recieved);
            if (bytes_written2 < 0) {
                perror("Failed writting to usb2");
            }
        }
    }

    close(fd1);
    close(fd2);
    close(client_sockfd);

    return 0;

}


int info(char *args[], int client_sockfd){

    char *path = args[1];
    char buffer[BUFFER_SIZE] = {0};

    printf("Operation: INFO\n");
    printf("Filename: %s\n", path);

    char path1[strlen(usb1) + strlen(path) + 1];
    char path2[strlen(usb2) + strlen(path) + 1];

    strcpy(path1, usb1);
    strcat(path1, path);
    strcpy(path2, usb2);
    strcat(path2, path);

    struct stat file_stat;

    if (stat(path1, &file_stat) < 0) {
        perror("Failed getting file stats from usb1\n");
        return -1;
    }
    else if (stat(path2, &file_stat) < 0) {
        perror("Failed getting file stats from usb2\n");
        return -1;
    }
    else {
        strcat(buffer, "File server currently unavailable\n");
        if (send(client_sockfd, buffer, strlen(buffer), 0) < 0) {
            perror("Error sending server status message");
        }
        return -1;
    }

    char permissions[BUFFER_SIZE];
    char owner[BUFFER_SIZE];
    char size[BUFFER_SIZE];
    char access[BUFFER_SIZE];
    char modification[BUFFER_SIZE];
    char change[BUFFER_SIZE];

    sprintf(permissions, "File permissions: %o\n", file_stat.st_mode & 0777);
    sprintf(owner, "Owner: %d\n", file_stat.st_uid);
    sprintf(size, "Size: %lld bytes\n", file_stat.st_size);
    sprintf(access, "Last access time: %s", ctime(&file_stat.st_atime));
    sprintf(modification, "Last modification time: %s", ctime(&file_stat.st_mtime));
    sprintf(change, "Last status change time: %s", ctime(&file_stat.st_ctime));

    strcat(buffer, permissions);
    strcat(buffer, owner);
    strcat(buffer, size);
    strcat(buffer, access);
    strcat(buffer, modification);
    strcat(buffer, change);

    if (send(client_sockfd, buffer, strlen(buffer), 0) < 0) {
        perror("Error sending file");
        return -1;
    }

    return 0;

}

int md(char *args[], int client_sockfd){

    char *path = args[1];

    printf("Operation: MD\n");
    printf("Directory: %s\n", path);

    char path1[strlen(usb1) + strlen(path) + 1];
    char path2[strlen(usb2) + strlen(path) + 1];

    strcpy(path1, usb1);
    strcat(path1, path);
    strcpy(path2, usb2);
    strcat(path2, path);

    int result1 = mkdir(path1, 0777);
    int result2 = mkdir(path2, 0777);

    char buffer[BUFFER_SIZE] = {0};

    if ( (result1 == 0) || (result2 == 0) ) {
        strcat(buffer, "Success\n");
    } else {
        strcat(buffer, "Failed\n");
    }

    if (send(client_sockfd, buffer, strlen(buffer), 0) < 0) {
        perror("Error sending status");
        return -1;
    }

    return 0;

}


int rm(char *args[], int client_sockfd){

    char *path = args[1];

    printf("Operation: RM\n");
    printf("Filename: %s\n", path);

    char path1[strlen(usb1) + strlen(path) + 1];
    char path2[strlen(usb2) + strlen(path) + 1];

    strcpy(path1, usb1);
    strcat(path1, path);
    strcpy(path2, usb2);
    strcat(path2, path);

    int result1 = remove(path1);
    int result2 = remove(path2);

    char buffer[BUFFER_SIZE] = {0};

    if ( (result1 == 0) || (result2 == 0) ) {
        strcat(buffer, "Success\n");
    } else {
        strcat(buffer, "Failed\n");
    }

    if (send(client_sockfd, buffer, strlen(buffer), 0) < 0) {
        perror("Error sending status");
        return -1;
    }

    return 0;

}


int get(char *args[], int client_sockfd){

    char *path = args[1];

    printf("Operation: GET\n");
    printf("Filename: %s\n", path);

    char path1[strlen(usb1) + strlen(path) + 1];
    char path2[strlen(usb2) + strlen(path) + 1];

    strcpy(path1, usb1);
    strcat(path1, path);
    strcpy(path2, usb2);
    strcat(path2, path);

    FILE *fp = fopen(path1, "r");
    if (fp == NULL){
        fp = fopen(path2, "r");
        if (fp == NULL){
            char buffer[BUFFER_SIZE] = {0};
            strcat(buffer, "File does not exist\n");
            if (send(client_sockfd, buffer, strlen(buffer), 0) < 0) {
                perror("Error sending server status message");
            }
            return -1;
        }
    }

    int bytes_sent = 0;
    int total_bytes_sent = 0;
    char filebuffer[1024] = {0};

    while ((bytes_sent = fread(filebuffer, 1, BUFFER_SIZE, fp)) > 0) {
        total_bytes_sent += bytes_sent;
        if (send(client_sockfd, filebuffer, bytes_sent, 0) < 0) {
            perror("Error sending file");
            return -1;
        }
        memset(filebuffer, 0, BUFFER_SIZE);
    }

    return 0;

}



int main(){

    Config config;
    parse_config("config.ini", &config);

    // use config values
    printf("port=%d\n", config.port);
    printf("hostname=%s\n", config.host);
    printf("usb1=%s\n", config.usb1);
    printf("usb2=%s\n", config.usb2);

    port = config.port;
    host = config.host;
    usb1 = config.usb1;
    usb2 = config.usb2;

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

    pthread_mutex_init(&lock, NULL);

    while(1) {

        client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_size);

        if (client_sockfd < 0)
        {
            printf("Can't accept\n");
            continue;
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

        pthread_mutex_lock(&lock);

        if (strcmp(args[0], "PUT") == 0) {
            put(args, client_sockfd);
        }
        else if (strcmp(args[0], "GET") == 0) {
            get(args, client_sockfd);
        }
        else if (strcmp(args[0], "INFO") == 0) {
            info(args, client_sockfd);
        }
        else if (strcmp(args[0], "MD") == 0) {
            md(args, client_sockfd);
        }
        else if (strcmp(args[0], "RM") == 0) {
            rm(args, client_sockfd);
        }

        pthread_mutex_unlock(&lock);
        
        close(client_sockfd);

    }

    close(sockfd);

    pthread_mutex_destroy(&lock);

    return 0;
}