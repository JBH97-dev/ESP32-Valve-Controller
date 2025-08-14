#include "valve_model.h"
#include "esp_log.h"
#include "esp_system.h"
#include <string.h>
#include "esp_timer.h"
#include "config_manager.h"
#include "cJSON.h"

static const char *TAG = "valve_model";
static valve_system_t valve_system = {0};
static bool is_initialized = false;

esp_err_t valve_model_load_config(void);

esp_err_t valve_model_init(uint8_t valve_count)
{
    if (valve_count > MAX_VALVE_COUNT) {
        ESP_LOGE(TAG, "Valve count %d exceeds maximum %d", valve_count, MAX_VALVE_COUNT);
        return ESP_ERR_INVALID_ARG;
    }

    memset(&valve_system, 0, sizeof(valve_system_t));
    valve_system.valve_count = valve_count;
    valve_system.system_enabled = true;
    valve_system.system_uptime = 0;

    esp_err_t err = valve_model_load_config();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to load valve config, using defaults");
        // Initialize default valve configurations
        
        for (uint8_t i = 0; i < valve_count; i++) {
            valve_t *valve = &valve_system.valves[i];
            valve->id = i + 1;
            snprintf(valve->name, MAX_VALVE_NAME_LENGTH, "Valve %d", i + 1);
            valve->state = VALVE_STATE_OFF;
            valve->mode = VALVE_MODE_MANUAL;
            valve->total_runtime_seconds = 0;
            valve->last_activated_timestamp = 0;
            valve->is_enabled = true;
            valve->is_inverted = false;
        }
    }

    is_initialized = true;
    ESP_LOGI(TAG, "Valve model initialized with %d valves", valve_count);
    return ESP_OK;
}

esp_err_t valve_model_load_config(void) {
    ESP_LOGI(TAG, "Loading valve configuration from config.json");

    cJSON *valves_json = NULL;
    cJSON *valve_json = NULL;

    cJSON *config_json = config_manager_get_json();
    if (config_json == NULL) {
        ESP_LOGE(TAG, "Failed to get config JSON");
        return ESP_FAIL;
    }

    valves_json = cJSON_GetObjectItemCaseSensitive(config_json, "valves");
    if (!cJSON_IsObject(valves_json)) {
        ESP_LOGE(TAG, "valves is not an object");
        return ESP_FAIL;
    }

    int valve_index = 0;
    cJSON_ArrayForEach(valve_json, valves_json) {
        if (valve_index >= valve_system.valve_count) {
            ESP_LOGW(TAG, "Too many valves in config.json, ignoring");
            break;
        }

        valve_t *valve = &valve_system.valves[valve_index];

        const char *name = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(valve_json, "name"));
        if (name != NULL) {
            strncpy(valve->name, name, MAX_VALVE_NAME_LENGTH - 1);
            valve->name[MAX_VALVE_NAME_LENGTH - 1] = '\0';
        }

        int open_time = config_get_int("open_time", 60);
        valve->open_time = open_time;

        bool is_scheduled = config_get_bool("is_scheduled", false);
        valve->is_scheduled = is_scheduled;

        ESP_LOGI(TAG, "Loaded config for valve %d: name=%s, open_time=%d, is_scheduled=%d",
                 valve->id, valve->name, valve->open_time, valve->is_scheduled);
        valve_index++;
    }

    return ESP_OK;
}

valve_system_t* valve_model_get_system(void)
{
    if (!is_initialized) {
        ESP_LOGE(TAG, "Valve model not initialized");
        return NULL;
    }
    return &valve_system;
}

valve_t* valve_model_get_valve(uint8_t valve_id)
{
    if (!is_initialized) {
        ESP_LOGE(TAG, "Valve model not initialized");
        return NULL;
    }

    if (valve_id < 1 || valve_id > valve_system.valve_count) {
        ESP_LOGE(TAG, "Invalid valve ID: %d", valve_id);
        return NULL;
    }

    return &valve_system.valves[valve_id - 1];
}

esp_err_t valve_model_set_valve_state(uint8_t valve_id, valve_state_t state)
{
    valve_t *valve = valve_model_get_valve(valve_id);
    if (valve == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!valve->is_enabled && state == VALVE_STATE_ON) {
        ESP_LOGW(TAG, "Cannot turn on disabled valve %d", valve_id);
        return ESP_ERR_INVALID_STATE;
    }

    valve_state_t old_state = valve->state;
    valve->state = state;

    // Update runtime tracking
    if (old_state == VALVE_STATE_OFF && state == VALVE_STATE_ON) {
        valve->last_activated_timestamp = esp_timer_get_time() / 1000000; // Convert to seconds
    } else if (old_state == VALVE_STATE_ON && state == VALVE_STATE_OFF) {
        if (valve->last_activated_timestamp > 0) {
            uint32_t current_time = esp_timer_get_time() / 1000000;
            valve->total_runtime_seconds += (current_time - valve->last_activated_timestamp);
        }
    }

    ESP_LOGI(TAG, "Valve %d state changed from %d to %d", valve_id, old_state, state);
    return ESP_OK;
}

esp_err_t valve_model_set_valve_mode(uint8_t valve_id, valve_mode_t mode)
{
    valve_t *valve = valve_model_get_valve(valve_id);
    if (valve == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    valve->mode = mode;
    ESP_LOGI(TAG, "Valve %d mode changed to %d", valve_id, mode);
    return ESP_OK;
}

esp_err_t valve_model_set_system_enabled(bool enabled)
{
    if (!is_initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    valve_system.system_enabled = enabled;
    ESP_LOGI(TAG, "System enabled state changed to %s", enabled ? "true" : "false");
    return ESP_OK;
}

uint8_t valve_model_get_active_valve_count(void)
{
    if (!is_initialized) {
        return 0;
    }

    uint8_t active_count = 0;
    for (uint8_t i = 0; i < valve_system.valve_count; i++) {
        if (valve_system.valves[i].state == VALVE_STATE_ON) {
            active_count++;
        }
    }
    return active_count;
}
