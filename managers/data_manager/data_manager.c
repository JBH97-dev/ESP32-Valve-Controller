#include "data_manager.h"


static data_manager_state_t get_data_manager_state();
static void set_data_manager_state(data_manager_state_t state);

static data_manager_state_t data_manager_state;

data_manager_t* data_manager_init() {
    
    //alloc data_manager struct
    data_manager_t* data_manager = (data_manager_t*)malloc(sizeof(data_manager_t));
    //assign pointer functions
    data_manager->get_state = get_data_manager_state;
    data_manager->set_state = set_data_manager_state;

    // Initialize valves
    for (int i = 0; i < 10; i++) {
        data_manager_state.valves[i].id = i;
        data_manager_state.valves[i].is_open = false;
    }

    // Initialize system state
    data_manager_state.system_state.temperature = 25.0;
    data_manager_state.system_state.humidity = 60.0;
    
    return data_manager;
}

data_manager_state_t get_data_manager_state() {
    return data_manager_state;
}

void set_data_manager_state(data_manager_state_t state) {
    data_manager_state = state;
}
