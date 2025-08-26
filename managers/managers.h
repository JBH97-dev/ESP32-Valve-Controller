#ifndef MANAGERS_H
#define MANAGERS_H

#include "esp_err.h"
#include "config_manager.h"
#include "data_manager.h"
#include "spiffs_manager.h"

typedef struct managers_s{
    data_manager_t data_manager;
    config_manager_t config_manager;
    spiffs_manager_t spiffs_manager;
} managers_t;

#endif