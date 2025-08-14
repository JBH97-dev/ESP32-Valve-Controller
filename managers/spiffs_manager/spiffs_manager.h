#ifndef SPIFFS_MANAGER_H
#define SPIFFS_MANAGER_H

#include <stdio.h>

FILE *spiffs_open_file(const char *path, const char *mode);
FILE *spiffs_create_file(const char *path);

#endif
