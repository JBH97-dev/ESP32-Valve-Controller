#ifndef VALVE_MODEL_H
#define VALVE_MODEL_H

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>
#include <endian.h>

#define MAX_VALVE_COUNT 8
#define MAX_VALVE_NAME_LENGTH 32

typedef enum {
    VALVE_STATE_OFF = 0,
    VALVE_STATE_ON = 1,
    VALVE_STATE_ERROR = 2
} valve_state_t;

typedef enum {
    VALVE_MODE_MANUAL = 0,
    VALVE_MODE_AUTO = 1,
    VALVE_MODE_SCHEDULED = 2
} valve_mode_t;

typedef struct {
    uint8_t id;
    char name[MAX_VALVE_NAME_LENGTH];
    uint8_t gpio_pin;
    valve_state_t state;
    valve_mode_t mode;
    uint32_t total_runtime_seconds;
    uint32_t last_activated_timestamp;
    bool is_enabled;
    bool is_inverted;  // For normally closed valves
    int open_time;
    bool is_scheduled;
} valve_t;

typedef struct {
    valve_t valves[MAX_VALVE_COUNT];
    uint8_t valve_count;
    bool system_enabled;
    uint32_t system_uptime;
} valve_system_t;

// Model function declarations
esp_err_t valve_model_init(uint8_t valve_count);
valve_system_t* valve_model_get_system(void);
valve_t* valve_model_get_valve(uint8_t valve_id);
esp_err_t valve_model_set_valve_state(uint8_t valve_id, valve_state_t state);
esp_err_t valve_model_set_valve_mode(uint8_t valve_id, valve_mode_t mode);
esp_err_t valve_model_set_system_enabled(bool enabled);
uint8_t valve_model_get_active_valve_count(void);

#endif // VALVE_MODEL_H
