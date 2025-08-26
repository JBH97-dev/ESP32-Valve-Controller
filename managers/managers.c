#include "managers.h"
#include "config_manager.h"
#include "data_manager.h"
#include "esp_err.h"
#include "spiffs_manager.h"


esp_err_t managers_init(managers_t *managers)
{
    esp_err_t ret = ESP_FAIL;

    spiffs_manager_t* sm = spiffs_manager_init();
    config_manager_t* cm = config_manager_init();
    data_manager_t* dm = data_manager_init();

    managers->config_manager = cm;
    managers->spiffs_manager = sm;
    managers->data_manager = dm;
    
    if(managers->config_manager && managers->spiffs_manager && managers->data_manager){
        ret = ESP_OK;
    }

    return ret;
}