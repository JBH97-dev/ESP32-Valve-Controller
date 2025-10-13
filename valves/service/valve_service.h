#ifndef VALVE_SERVICE_H
#define VALVE_SERVICE_H

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <string.h>
#include "esp_err.h"
#include "valve_model.h"

// Service function declarations
esp_err_t valve_service_init(uint8_t valve_count);
esp_err_t valve_service_turn_on(uint8_t valve_id);
esp_err_t valve_service_turn_off(uint8_t valve_id);
esp_err_t valve_service_toggle(uint8_t valve_id);
esp_err_t valve_service_turn_all_on(void);
esp_err_t valve_service_turn_all_off(void);
esp_err_t valve_service_emergency_stop(void);
esp_err_t valve_service_set_valve_name(uint8_t valve_id, const char* name);
esp_err_t valve_service_enable_valve(uint8_t valve_id, bool enabled);
valve_system_t* valve_service_get_system_status(void);
bool valve_service_is_system_healthy(void);

// Hardware abstraction
esp_err_t valve_service_hardware_init(void);
esp_err_t valve_service_hardware_set_pin(uint8_t valve_id, bool state);

#endif // VALVE_SERVICE_H
