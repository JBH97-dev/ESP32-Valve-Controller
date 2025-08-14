#include "spiffs_manager.h"
#include "esp_log.h"
#include "esp_spiffs.h"

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
