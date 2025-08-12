#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <stdio.h>
#include <stdbool.h>

// Initialize the config manager (mount SPIFFS, load config)
bool config_manager_init();

// Load the configuration from the file
bool config_load();

// Save the configuration to the file
bool config_save();

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
