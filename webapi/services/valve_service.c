#include "valve_service.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <string.h>

static const char *TAG = "valve_service";
static SemaphoreHandle_t valve_mutex = NULL;
static bool service_initialized = false;

esp_err_t valve_service_init(uint8_t valve_count)
{
    ESP_LOGI(TAG, "Initializing valve service with %d valves", valve_count);

    // Initialize the valve model
    esp_err_t ret = valve_model_init(valve_count);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize valve model");
        return ret;
    }

    // Initialize hardware
    ret = valve_service_hardware_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize valve hardware");
        return ret;
    }
    
    // Load valve configurations from config.json
    


    // Create mutex for thread safety
    valve_mutex = xSemaphoreCreateMutex();
    if (valve_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create valve mutex");
        return ESP_ERR_NO_MEM;
    }

    service_initialized = true;
    ESP_LOGI(TAG, "Valve service initialized successfully");
    return ESP_OK;
}

esp_err_t valve_service_turn_on(uint8_t valve_id)
{
    if (!service_initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    if (xSemaphoreTake(valve_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to acquire valve mutex");
        return ESP_ERR_TIMEOUT;
    }

    esp_err_t ret = ESP_OK;
    valve_system_t *system = valve_model_get_system();
    
    if (!system->system_enabled) {
        ESP_LOGW(TAG, "Cannot turn on valve %d - system disabled", valve_id);
        ret = ESP_ERR_INVALID_STATE;
        goto cleanup;
    }

    valve_t *valve = valve_model_get_valve(valve_id);
    if (valve == NULL) {
        ret = ESP_ERR_INVALID_ARG;
        goto cleanup;
    }

    if (!valve->is_enabled) {
        ESP_LOGW(TAG, "Cannot turn on disabled valve %d", valve_id);
        ret = ESP_ERR_INVALID_STATE;
        goto cleanup;
    }

    // Update model state
    ret = valve_model_set_valve_state(valve_id, VALVE_STATE_ON);
    if (ret != ESP_OK) {
        goto cleanup;
    }

    // Update hardware
    ret = valve_service_hardware_set_pin(valve_id, true);
    if (ret != ESP_OK) {
        // Rollback model state on hardware failure
        valve_model_set_valve_state(valve_id, VALVE_STATE_ERROR);
        ESP_LOGE(TAG, "Failed to turn on valve %d hardware", valve_id);
        goto cleanup;
    }

    ESP_LOGI(TAG, "Valve %d turned ON", valve_id);

cleanup:
    xSemaphoreGive(valve_mutex);
    return ret;
}

esp_err_t valve_service_turn_off(uint8_t valve_id)
{
    if (!service_initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    if (xSemaphoreTake(valve_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to acquire valve mutex");
        return ESP_ERR_TIMEOUT;
    }

    esp_err_t ret = ESP_OK;
    valve_t *valve = valve_model_get_valve(valve_id);
    if (valve == NULL) {
        ret = ESP_ERR_INVALID_ARG;
        goto cleanup;
    }

    // Update model state
    ret = valve_model_set_valve_state(valve_id, VALVE_STATE_OFF);
    if (ret != ESP_OK) {
        goto cleanup;
    }

    // Update hardware
    ret = valve_service_hardware_set_pin(valve_id, false);
    if (ret != ESP_OK) {
        valve_model_set_valve_state(valve_id, VALVE_STATE_ERROR);
        ESP_LOGE(TAG, "Failed to turn off valve %d hardware", valve_id);
        goto cleanup;
    }

    ESP_LOGI(TAG, "Valve %d turned OFF", valve_id);

cleanup:
    xSemaphoreGive(valve_mutex);
    return ret;
}

esp_err_t valve_service_toggle(uint8_t valve_id)
{
    valve_t *valve = valve_model_get_valve(valve_id);
    if (valve == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (valve->state == VALVE_STATE_ON) {
        return valve_service_turn_off(valve_id);
    } else {
        return valve_service_turn_on(valve_id);
    }
}

esp_err_t valve_service_turn_all_on(void)
{
    if (!service_initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    valve_system_t *system = valve_model_get_system();
    if (system == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = ESP_OK;
    for (uint8_t i = 1; i <= system->valve_count; i++) {
        esp_err_t valve_ret = valve_service_turn_on(i);
        if (valve_ret != ESP_OK) {
            ESP_LOGW(TAG, "Failed to turn on valve %d: %s", i, esp_err_to_name(valve_ret));
            ret = valve_ret; // Keep last error
        }
    }

    ESP_LOGI(TAG, "All valves turn on command completed");
    return ret;
}

esp_err_t valve_service_turn_all_off(void)
{
    if (!service_initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    valve_system_t *system = valve_model_get_system();
    if (system == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = ESP_OK;
    for (uint8_t i = 1; i <= system->valve_count; i++) {
        esp_err_t valve_ret = valve_service_turn_off(i);
        if (valve_ret != ESP_OK) {
            ESP_LOGW(TAG, "Failed to turn off valve %d: %s", i, esp_err_to_name(valve_ret));
            ret = valve_ret; // Keep last error
        }
    }

    ESP_LOGI(TAG, "All valves turn off command completed");
    return ret;
}

esp_err_t valve_service_emergency_stop(void)
{
    ESP_LOGW(TAG, "EMERGENCY STOP activated - turning off all valves");
    
    // Force system disable
    valve_model_set_system_enabled(false);
    
    // Turn off all valves immediately
    esp_err_t ret = valve_service_turn_all_off();
    
    ESP_LOGW(TAG, "Emergency stop completed");
    return ret;
}

esp_err_t valve_service_set_valve_name(uint8_t valve_id, const char* name)
{
    if (name == NULL || strlen(name) >= MAX_VALVE_NAME_LENGTH) {
        return ESP_ERR_INVALID_ARG;
    }

    valve_t *valve = valve_model_get_valve(valve_id);
    if (valve == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    strncpy(valve->name, name, MAX_VALVE_NAME_LENGTH - 1);
    valve->name[MAX_VALVE_NAME_LENGTH - 1] = '\0';
    
    ESP_LOGI(TAG, "Valve %d renamed to '%s'", valve_id, name);
    return ESP_OK;
}

esp_err_t valve_service_enable_valve(uint8_t valve_id, bool enabled)
{
    valve_t *valve = valve_model_get_valve(valve_id);
    if (valve == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    valve->is_enabled = enabled;
    
    // If disabling, turn off the valve
    if (!enabled && valve->state == VALVE_STATE_ON) {
        valve_service_turn_off(valve_id);
    }

    ESP_LOGI(TAG, "Valve %d %s", valve_id, enabled ? "enabled" : "disabled");
    return ESP_OK;
}

valve_system_t* valve_service_get_system_status(void)
{
    return valve_model_get_system();
}

bool valve_service_is_system_healthy(void)
{
    valve_system_t *system = valve_model_get_system();
    if (system == NULL) {
        return false;
    }

    // Check for any valves in error state
    for (uint8_t i = 0; i < system->valve_count; i++) {
        if (system->valves[i].state == VALVE_STATE_ERROR) {
            return false;
        }
    }

    return system->system_enabled;
}

esp_err_t valve_service_hardware_init(void)
{
    ESP_LOGI(TAG, "Initializing valve hardware");

    valve_system_t *system = valve_model_get_system();
    if (system == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    for (uint8_t i = 0; i < system->valve_count; i++) {
        valve_t *valve = &system->valves[i];
        
        gpio_reset_pin(valve->gpio_pin);
        gpio_set_direction(valve->gpio_pin, GPIO_MODE_OUTPUT);
        gpio_set_level(valve->gpio_pin, valve->is_inverted ? 1 : 0); // Start with valve off
        
        ESP_LOGI(TAG, "Initialized GPIO %d for valve %d", valve->gpio_pin, valve->id);
    }

    return ESP_OK;
}

esp_err_t valve_service_hardware_set_pin(uint8_t valve_id, bool state)
{
    valve_t *valve = valve_model_get_valve(valve_id);
    if (valve == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    // Apply inversion logic if needed
    bool gpio_level = valve->is_inverted ? !state : state;
    gpio_set_level(valve->gpio_pin, gpio_level ? 1 : 0);
    
    return ESP_OK;
}
