#ifndef MANAGERS_H
#define MANAGERS_H

/* Include */
#include "esp_err.h"
#include "config_manager.h"
#include "data_manager.h"
#include "spiffs_manager.h"

/* TypeDef Declerations*/
typedef struct managers_s{
    data_manager_t* data_manager;
    config_manager_t* config_manager;
    spiffs_manager_t* spiffs_manager;
} managers_t;

/* Functions declerations */
esp_err_t managers_init(managers_t*);

#endif