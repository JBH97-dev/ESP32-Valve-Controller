#ifndef SPIFFS_MANAGER_H
#define SPIFFS_MANAGER_H

#include "esp_err.h"
#include <stdio.h>
#include "esp_log.h"
#include "esp_spiffs.h"
#include <dirent.h>

typedef struct spiffs_manager_s{

    void        (*list_spiffs_files_to_console)(void);
     
} spiffs_manager_t;

spiffs_manager_t* spiffs_manager_init();

FILE *spiffs_open_file(const char *path, const char *mode);
FILE *spiffs_create_file(const char *path);

esp_err_t spiffs_read_valves_json(char **json_string);
esp_err_t spiffs_write_valves_json(const char *json_string);

#endif
