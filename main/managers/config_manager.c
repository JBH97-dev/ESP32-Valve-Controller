#include "config_manager.h"
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_spiffs.h"
#include "cJSON.h"

static const char *TAG = "config_manager";
static const char *CONFIG_FILE = "/spiffs/config.json";

static cJSON *config_json = NULL;

bool config_manager_init() {
    ESP_LOGI(TAG, "Initializing config manager");

    if (!config_load()) {
        ESP_LOGI(TAG, "Failed to load config, creating default");
        config_json = cJSON_CreateObject();
        config_save();
    }

    char *json_string = cJSON_Print(config_json);
    ESP_LOGW(TAG, "Config: %s", json_string);
    cJSON_free(json_string);

    return true;
}

bool config_load() {
    ESP_LOGI(TAG, "Loading config from %s", CONFIG_FILE);

    FILE *f = fopen(CONFIG_FILE, "r");
    if (f == NULL) {
        ESP_LOGI(TAG, "Config file not found");
        return false;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = '\0';

    config_json = cJSON_Parse(string);
    free(string);

    if (config_json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            ESP_LOGE(TAG, "Error before: %s", error_ptr);
        }
        return false;
    }

    return true;
}

bool config_save() {
    ESP_LOGI(TAG, "Saving config to %s", CONFIG_FILE);

    FILE *f = fopen(CONFIG_FILE, "w");
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
