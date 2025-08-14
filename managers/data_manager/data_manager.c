#include "data_manager.h"

#include <stdio.h>
#include <string.h>

static data_manager_state_t data_manager_state;

void init_data_manager() {
    // Initialize valves
    for (int i = 0; i < 10; i++) {
        data_manager_state.valves[i].id = i;
        data_manager_state.valves[i].is_open = false;
    }

    // Initialize system state
    data_manager_state.system_state.temperature = 25.0;
    data_manager_state.system_state.humidity = 60.0;
}

data_manager_state_t get_data_manager_state() {
    return data_manager_state;
}

void set_data_manager_state(data_manager_state_t state) {
    data_manager_state = state;
}
