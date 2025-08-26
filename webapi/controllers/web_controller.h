#ifndef WEB_CONTROLLER_H
#define WEB_CONTROLLER_H

#include "esp_http_server.h"
#include "managers.h"
#define FILE_PATH_MAX 256

// Web controller function declarations
esp_err_t web_controller_register_routes(httpd_handle_t server,managers_t* managers);
esp_err_t web_controller_serve_index(httpd_req_t *req);

#endif // WEB_CONTROLLER_H
