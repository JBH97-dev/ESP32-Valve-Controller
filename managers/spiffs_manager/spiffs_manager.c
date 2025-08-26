#include "spiffs_manager.h"


static const char *TAG = "spiffs_manager";

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
