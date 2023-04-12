#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

void parse_config(const char *filename, Config *config) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        // handle error
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char key[256];
        char value[256];
        int n = sscanf(line, "%255[^=]=%255[^\n]", key, value);
        if (n != 2) {
            // ignore invalid lines
            continue;
        }

        if (strcmp(key, "port") == 0) {
            config->port = atoi(value);
        } else if (strcmp(key, "host") == 0) {
            strncpy(config->host, value, sizeof(config->host));
            config->host[sizeof(config->host) - 1] = '\0'; // ensure null-terminated
        } else if (strcmp(key, "usb1") == 0) {
            strncpy(config->usb1, value, sizeof(config->usb1));
            config->usb1[sizeof(config->usb1) - 1] = '\0'; // ensure null-terminated
        } else if (strcmp(key, "usb2") == 0) {
            strncpy(config->usb2, value, sizeof(config->usb2));
            config->usb2[sizeof(config->usb2) - 1] = '\0'; // ensure null-terminated
        } else if (strcmp(key, "logfile") == 0) {
            strncpy(config->logfile, value, sizeof(config->logfile));
            config->logfile[sizeof(config->logfile) - 1] = '\0'; // ensure null-terminated
        } else {
            // ignore unrecognized keys
        }
    }

    fclose(file);
}
