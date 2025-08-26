#include "spiffs_manager.h"
#include <stdlib.h>


static const char *TAG = "spiffs_manager";

spiffs_manager_t* spiffs_manager_init(){
    
    spiffs_manager_t* spiffs_manager = malloc(sizeof(spiffs_manager_t));

    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
            return NULL;
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
            return NULL;
        }
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        return NULL;
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    return spiffs_manager;
}


FILE *spiffs_open_file(const char *path, const char *mode) {
    ESP_LOGI(TAG, "Opening file %s in mode %s", path, mode);
    FILE *f = fopen(path, mode);
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file %s", path);
    }
    return f;
}

FILE *spiffs_create_file(const char *path) {
    ESP_LOGI(TAG, "Creating file %s", path);
    FILE *f = fopen(path, "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to create file %s", path);
    }
    return f;
}

void list_spiffs_files_to_console()
{
    ESP_LOGI(TAG, "Listing files in /spiffs");

    DIR *dir = opendir("/spiffs");
    if (dir == NULL) {
        ESP_LOGE(TAG, "Failed to open /spiffs directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        ESP_LOGI(TAG, "File: %s", entry->d_name);
    }
    closedir(dir);
}

esp_err_t spiffs_read_valves_json(char **json_string) {
    FILE *f = spiffs_open_file("/spiffs/valves.json", "r");
    if (f == NULL) {
        return ESP_FAIL;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    *json_string = malloc(fsize + 1);
    if (*json_string == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for JSON string");
        fclose(f);
        return ESP_FAIL;
    }

    fread(*json_string, 1, fsize, f);
    fclose(f);
    (*json_string)[fsize] = 0; // Null-terminate the string

    ESP_LOGI(TAG, "Read from valves.json: %s", *json_string);
    return ESP_OK;
}

esp_err_t spiffs_write_valves_json(const char *json_string) {
    FILE *f = spiffs_open_file("/spiffs/valves.json", "w");
    if (f == NULL) {
        return ESP_FAIL;
    }

    if (fprintf(f, "%s", json_string) < 0) {
        ESP_LOGE(TAG, "Failed to write to file");
        fclose(f);
        return ESP_FAIL;
    }
    fclose(f);

    ESP_LOGI(TAG, "Written to valves.json: %s", json_string);
    return ESP_OK;
}
