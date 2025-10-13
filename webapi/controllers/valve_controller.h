#ifndef VALVE_CONTROLLER_H
#define VALVE_CONTROLLER_H

#include "esp_http_server.h"
#include "managers.h"
// Controller function declarations
esp_err_t valve_controller_register_routes(httpd_handle_t server);

// Route handlers
esp_err_t valve_controller_get_status(httpd_req_t *req);
esp_err_t valve_controller_control_valve(httpd_req_t *req);
esp_err_t valve_controller_control_all_valves(httpd_req_t *req);
esp_err_t valve_controller_get_valve_details(httpd_req_t *req);
esp_err_t valve_controller_update_valve_config(httpd_req_t *req);
esp_err_t valve_controller_emergency_stop(httpd_req_t *req);

#endif // VALVE_CONTROLLER_H
