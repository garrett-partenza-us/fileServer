#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

typedef struct {
    int port;
    char host[256];
    char usb1[256];
    char usb2[256];
    char logfile[256];
} Config;

void parse_config(const char *filename, Config *config);

#endif