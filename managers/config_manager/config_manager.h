#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <stdio.h>
#include <stdbool.h>
#include "cJSON.h"
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_spiffs.h"
#include "cJSON.h"
#include "spiffs_manager.h"

typedef struct config_manager_s{
    esp_err_t           (*save_change)(void);
    esp_err_t           (*load_config)(void);    
    cJSON*              (*get_config_json)(void);            
}config_manager_t;

//init function 
config_manager_t* config_manager_init();

//returns JSON of config.json
cJSON *config_manager_get_json() ;

// Get an integer configuration value
int config_get_int(const char *key, int default_value);

// Set an integer configuration value
void config_set_int(const char *key, int value);

// Get a float configuration value
float config_get_float(const char *key, float default_value);

// Set a float configuration value
void config_set_float(const char *key, float value);

// Get a boolean configuration value
bool config_get_bool(const char *key, bool default_value);

// Set a boolean configuration value
void config_set_bool(const char *key, bool value);

// Get a string configuration value
const char *config_get_string(const char *key, const char *default_value);

// Set a string configuration value
void config_set_string(const char *key, const char *value);


#endif // CONFIG_MANAGER_H
