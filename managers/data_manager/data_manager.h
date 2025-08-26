#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "esp_err.h"
#include <stdbool.h>
#include <stdlib.h>

typedef struct data_manager_valve_s{
    int id;
    bool is_open;
    // Add more valve properties here
} data_manager_valve_t;

typedef struct system_state_s{
    // Add system properties here
    float temperature;
    float humidity;
} system_state_t;

typedef struct data_manager_state_s{
    data_manager_valve_t valves[10]; // Assuming a maximum of 10 valves
    system_state_t system_state;
} data_manager_state_t;

typedef struct data_manager_s{
    
    //data manager function pointers
    data_manager_state_t    (*get_state)(void);
    void                    (*set_state)(data_manager_state_t state);
    esp_err_t               (*data_manager_init)(void);

} data_manager_t;

data_manager_t* data_manager_init();

#endif
