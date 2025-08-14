#include "config_manager.h"
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_spiffs.h"
#include "cJSON.h"
#include "spiffs_manager.h"

static const char *TAG = "config_manager";
static const char *CONFIG_FILE = "/spiffs/config.json";

static cJSON *config_json = NULL;

bool config_manager_init() {
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
            return false;
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
            return false;
        }
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        return false;
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    FILE* f = spiffs_open_file(CONFIG_FILE, "r");
    if (f == NULL) {
        ESP_LOGI(TAG, "Config file not found, creating default");
        config_json = cJSON_CreateObject();
        if (config_json == NULL) {
            ESP_LOGE(TAG, "Failed to create config JSON object");
            return false;
        }
        if (!config_save()) {
            ESP_LOGE(TAG, "Failed to save default config");
            cJSON_Delete(config_json);
            return false;
        }
    } else {
        fclose(f);
    }

    return true;
}

bool config_save() {
    ESP_LOGI(TAG, "Saving config to %s", CONFIG_FILE);

    FILE *f = spiffs_create_file(CONFIG_FILE);
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open config file for writing");
        return false;
    }

    char *string = cJSON_Print(config_json);
    fprintf(f, "%s", string);
    fclose(f);
    cJSON_free(string);

    return true;
}

int config_get_int(const char *key, int default_value) {
    cJSON *item = cJSON_GetObjectItemCaseSensitive(config_json, key);
    if (cJSON_IsNumber(item)) {
        return (int)item->valuedouble;
    }
    return default_value;
}

void config_set_int(const char *key, int value) {
    cJSON *item = cJSON_GetObjectItemCaseSensitive(config_json, key);
    if (item) {
        cJSON_SetNumberValue(item, value);
    } else {
        cJSON_AddItemToObject(config_json, key, cJSON_CreateNumber(value));
    }
}

float config_get_float(const char *key, float default_value) {
    cJSON *item = cJSON_GetObjectItemCaseSensitive(config_json, key);
    if (cJSON_IsNumber(item)) {
        return (float)item->valuedouble;
    }
    return default_value;
}

void config_set_float(const char *key, float value) {
    cJSON *item = cJSON_GetObjectItemCaseSensitive(config_json, key);
    if (item) {
        cJSON_SetNumberValue(item, value);
    } else {
        cJSON_AddItemToObject(config_json, key, cJSON_CreateNumber(value));
    }
}

bool config_get_bool(const char *key, bool default_value) {
    cJSON *item = cJSON_GetObjectItemCaseSensitive(config_json, key);
    if (cJSON_IsBool(item)) {
        return cJSON_IsTrue(item);
    }
    return default_value;
}

void config_set_bool(const char *key, bool value) {
    cJSON *item = cJSON_GetObjectItemCaseSensitive(config_json, key);
    if (item) {
        cJSON_SetBoolValue(item, value);
    } else {
        cJSON_AddItemToObject(config_json, key, value ? cJSON_CreateTrue() : cJSON_CreateFalse());
    }
}

const char *config_get_string(const char *key, const char *default_value) {
    cJSON *item = cJSON_GetObjectItemCaseSensitive(config_json, key);
    if (cJSON_IsString(item) && (item->valuestring != NULL)) {
        return item->valuestring;
    }
    return default_value;
}

void config_set_string(const char *key, const char *value) {
    cJSON *item = cJSON_GetObjectItemCaseSensitive(config_json, key);
    if (item) {
        cJSON_SetValuestring(item, value);
    } else {
        cJSON_AddItemToObject(config_json, key, cJSON_CreateString(value));
    }
}

cJSON *config_manager_get_json() {
    ESP_LOGI(TAG, "Loading config from %s", CONFIG_FILE);

    FILE *f = spiffs_open_file(CONFIG_FILE, "r");
    if (f == NULL) {
        ESP_LOGW(TAG, "Config file not found");
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = '\0';

    cJSON *json = cJSON_Parse(string);
    free(string);

    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            ESP_LOGE(TAG, "Error before: %s", error_ptr);
        }
        return config_json;
    }

    if (config_json != NULL) {
        cJSON_Delete(config_json);
    }
    config_json = json;

    return config_json;
}
