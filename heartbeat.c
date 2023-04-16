// Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "config.h"
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>


#define BUFFERSIZE 256 // Constant buffer size


// Declare global variables
char *logfile;
char *usb1;
char *usb2;


/**
 * Function name: copy
 * Description: Copy a file from one USB to another USB
 * Parameters: A pointer to a character array of the failed operation's path, integer of RAID storage device to copy to.
 * Returns: 0 (success), 1 (failure)
 **/
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


/**
 * Function name: update
 * Description: Syncronize USB devices on RAID storage following a failure. Iterate over log file and perform operations on out-of-sync USB.
 * Parameters: A pointer of character array pointers describing the names of the USB devices, integer of device which failed.
 * Returns: 0 (success), 1 (failure)
 **/
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

    // clear the file
    FILE *fp;
    fp = fopen(logfile, "w");
    fclose(fp);
    printf("[INFO]: File contents cleared successfully!\n");
    
    return 0;

}


int main() {

    // Parse config file
    Config config;
    parse_config("config.ini", &config);

    // Print configuration variables
    printf("[INFO]: usb1=%s\n", config.usb1);
    printf("[INFO]: usb2=%s\n", config.usb2);
    printf("[INFO]: logfile=%s\n", config.logfile);

    // Initialize global variables
    logfile = config.logfile;
    usb1 = config.usb1;
    usb2 = config.usb2;
    bool fix = false;
    int device;

    /**
     Enter an infinite loop for checking USB status every second.
     If USB goes offline, set 'fix' boolean flag to true, and wait for it to come back online.
     Once the USB comes back online, iterate over the logfile and update the failed USB back into a syncronized state.
     **/
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

