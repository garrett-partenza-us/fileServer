#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "config.h"
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>

#define BUFFERSIZE 256

char *logfile;
char *usb1;
char *usb2;

int copy(char *path, int device){

    char from[BUFFERSIZE];
    char to[BUFFERSIZE];

    if (device == 0) {
        strcpy(from, usb2);
        strcat(from, path);
        strcpy(to, usb1);
        strcat(to, path);
    }
    else {
        strcpy(from, usb1);
        strcat(from, path);
        strcpy(to, usb2);
        strcat(to, path);
    }

    printf("[INFO]: Copying from %s to %s/n", from, to);

    FILE *sourceFile, *destFile;
    char ch;

    sourceFile = fopen(from, "rb");
    if (sourceFile == NULL) {
        printf("[ERROR]: Could not open source file.\n");
        return 1;
    }

    destFile = fopen(to, "wb");
    if (destFile == NULL) {
        printf("[ERROR]:Could not open destination file.\n");
        fclose(sourceFile);
        return 1;
    }

    while ((ch = fgetc(sourceFile)) != EOF) {
        fputc(ch, destFile);
    }

    printf("[INFO]: File duplicated successfully.\n");

    fclose(sourceFile);
    fclose(destFile);

    return 0;

}


int update(char *dev_names[], int device) {

    FILE* file = fopen(logfile, "r");
    char line[256];

    if (file == 0) {
        perror("[ERROR]: Could not open logfile\n");
        exit(EXIT_FAILURE);
    }

    char *op;
    char *path;

    while (fgets(line, 256, file) != NULL) {
        op = strtok(line, " ");
        path = strtok(NULL, "\n");
        printf("[INFO]: op: %s, path: %s\n", op, path);
        if (strcmp(op, "PUT") == 0) {
            copy(path, device);
        }
        else if (strcmp(op, "RM") == 0) {
            char fullpath[strlen(usb1) + strlen(path) + 1];
            if (device == 0) {
                strcpy(fullpath, usb1);
            }
            else {
                strcpy(fullpath, usb2);
            }
            strcat(fullpath, path);
            printf("[INFO]: Attempting to remove %s\n", fullpath);
            int res = remove(fullpath);
            if (res < 0) {
                printf("[WARN]: Could not remove file\n");
            }
            else {
                printf("[INFO]: Successfully removed file\n");
            }
        }
        else if (strcmp(op, "MD") == 0) {
            char fullpath[strlen(usb1) + strlen(path) + 1];
            if (device == 0) {
                strcpy(fullpath, usb1);
            }
            else {
                strcpy(fullpath, usb2);
            }
            strcat(fullpath, path);
            printf("Creating directory %s\n", fullpath);
            int res = mkdir(fullpath, 0777);
            if (res < 0) {
                printf("[ERROR]: Failed creating directory\n");
            }
            else {
                printf("[INFO]: Successfully created directory\n");
            }
        }
        else {
            printf("[ERROR]: Unrecognized operation\n");
        }
    }
    
    // close file
    fclose(file);
    
    return 0;

}


int main() {

    Config config;
    parse_config("config.ini", &config);

    printf("[INFO]: usb1=%s\n", config.usb1);
    printf("[INFO]: usb2=%s\n", config.usb2);
    printf("[INFO]: logfile=%s\n", config.logfile);

    logfile = config.logfile;
    usb1 = config.usb1;
    usb2 = config.usb2;
    bool fix = false;
    int device;

    while (1) {

        struct stat sb;
        char *dev_names[] = {config.usb1, config.usb2};
        int num_devs = sizeof(dev_names) / sizeof(dev_names[0]);
        int i;

        for (i = 0; i < num_devs; i++) {
            if (stat(dev_names[i], &sb) == 0) {
                printf(".\n");
                if (fix) {
                    if (device == i) {
                        update(dev_names, device);
                        fix = false;
                    }
                }
            }
            else {
                printf("[WARN]: device not detected: %s\n", dev_names[i]);
                fix = true;
                device = i;
            }
        }

        // Wait for one second
        sleep(1);
    }
}

