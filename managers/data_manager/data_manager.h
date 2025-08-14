#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <stdbool.h>

typedef struct {
    int id;
    bool is_open;
    // Add more valve properties here
} data_manager_valve_t;

typedef struct {
    // Add system properties here
    float temperature;
    float humidity;
} system_state_t;

typedef struct {
    data_manager_valve_t valves[10]; // Assuming a maximum of 10 valves
    system_state_t system_state;
} data_manager_state_t;

void init_data_manager();
data_manager_state_t get_data_manager_state();
void set_data_manager_state(data_manager_state_t state);

#endif
