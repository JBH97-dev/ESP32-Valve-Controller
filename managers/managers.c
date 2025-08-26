#include "managers.h"
#include "config_manager.h"
#include "data_manager.h"
#include "esp_err.h"
#include "spiffs_manager.h"

esp_err_t managers_init(void)
{
    spiffs_manager_init();
    config_manager_t* cm = config_manager_init();
    data_manager_t* dm = data_manager_init();

    return ESP_OK;
}