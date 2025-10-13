#include "valve_controller.h"
#include "cJSON.h"
#include "esp_log.h"
#include <string.h>
#include <stdlib.h>

static const char *TAG = "valve_controller";

// Helper functions
static esp_err_t send_json_response(httpd_req_t *req, cJSON *json, int status_code);
static esp_err_t send_error_response(httpd_req_t *req, const char *error_message, int status_code);
static int extract_valve_id_from_uri(const char *uri);
static cJSON* valve_to_json(valve_t *valve);
static cJSON* system_to_json(valve_system_t *system);

esp_err_t valve_controller_register_routes(httpd_handle_t server)
{
    ESP_LOGI(TAG, "Registering valve controller routes");

    // GET /api/status - Get system status
    httpd_uri_t status_uri = {
        .uri = "/api/status",
        .method = HTTP_GET,
        .handler = valve_controller_get_status,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &status_uri);

    // POST /api/valve/{id}/control - Control individual valve
    httpd_uri_t valve_control_uri = {
        .uri = "/api/valve/*/control",
        .method = HTTP_POST,
        .handler = valve_controller_control_valve,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &valve_control_uri);

    // GET /api/valve/{id} - Get valve details
    httpd_uri_t valve_details_uri = {
        .uri = "/api/valve/*",
        .method = HTTP_GET,
        .handler = valve_controller_get_valve_details,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &valve_details_uri);

    // PUT /api/valve/{id} - Update valve configuration
    httpd_uri_t valve_update_uri = {
        .uri = "/api/valve/*",
        .method = HTTP_PUT,
        .handler = valve_controller_update_valve_config,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &valve_update_uri);

    // POST /api/valves/all - Control all valves
    httpd_uri_t valves_all_uri = {
        .uri = "/api/valves/all",
        .method = HTTP_POST,
        .handler = valve_controller_control_all_valves,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &valves_all_uri);

    // POST /api/emergency-stop - Emergency stop
    httpd_uri_t emergency_stop_uri = {
        .uri = "/api/emergency-stop",
        .method = HTTP_POST,
        .handler = valve_controller_emergency_stop,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &emergency_stop_uri);

    ESP_LOGI(TAG, "Valve controller routes registered successfully");
    return ESP_OK;
}

esp_err_t valve_controller_get_status(httpd_req_t *req)
{
    ESP_LOGI(TAG, "GET /api/status");

    valve_system_t *system = valve_service_get_system_status();
    if (system == NULL) {
        return send_error_response(req, "System not initialized", 500);
    }

    cJSON *json = system_to_json(system);
    if (json == NULL) {
        return send_error_response(req, "Failed to create JSON response", 500);
    }

    // Add system health status
    cJSON_AddBoolToObject(json, "healthy", valve_service_is_system_healthy());
    cJSON_AddNumberToObject(json, "active_valves", valve_model_get_active_valve_count());

    return send_json_response(req, json, 200);
}

esp_err_t valve_controller_control_valve(httpd_req_t *req)
{
    int valve_id = extract_valve_id_from_uri(req->uri);
    if (valve_id <= 0) {
        return send_error_response(req, "Invalid valve ID", 400);
    }

    ESP_LOGI(TAG, "POST /api/valve/%d/control", valve_id);

    // Read request body
    char buf[200];
    int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
    if (ret <= 0) {
        return send_error_response(req, "Failed to read request body", 400);
    }
    buf[ret] = '\0';

    // Parse JSON
    cJSON *json = cJSON_Parse(buf);
    if (json == NULL) {
        return send_error_response(req, "Invalid JSON", 400);
    }

    cJSON *action_json = cJSON_GetObjectItem(json, "action");
    if (!cJSON_IsString(action_json)) {
        cJSON_Delete(json);
        return send_error_response(req, "Missing or invalid 'action' field", 400);
    }

    const char *action = action_json->valuestring;
    esp_err_t service_ret = ESP_OK;

    if (strcmp(action, "on") == 0) {
        service_ret = valve_service_turn_on(valve_id);
    } else if (strcmp(action, "off") == 0) {
        service_ret = valve_service_turn_off(valve_id);
    } else if (strcmp(action, "toggle") == 0) {
        service_ret = valve_service_toggle(valve_id);
    } else {
        cJSON_Delete(json);
        return send_error_response(req, "Invalid action. Use 'on', 'off', or 'toggle'", 400);
    }

    cJSON_Delete(json);

    if (service_ret != ESP_OK) {
        return send_error_response(req, "Failed to control valve", 500);
    }

    // Return updated valve status
    valve_t *valve = valve_model_get_valve(valve_id);
    cJSON *response = valve_to_json(valve);
    return send_json_response(req, response, 200);
}

esp_err_t valve_controller_control_all_valves(httpd_req_t *req)
{
    ESP_LOGI(TAG, "POST /api/valves/all");

    // Read request body
    char buf[200];
    int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
    if (ret <= 0) {
        return send_error_response(req, "Failed to read request body", 400);
    }
    buf[ret] = '\0';

    // Parse JSON
    cJSON *json = cJSON_Parse(buf);
    if (json == NULL) {
        return send_error_response(req, "Invalid JSON", 400);
    }

    cJSON *action_json = cJSON_GetObjectItem(json, "action");
    if (!cJSON_IsString(action_json)) {
        cJSON_Delete(json);
        return send_error_response(req, "Missing or invalid 'action' field", 400);
    }

    const char *action = action_json->valuestring;
    esp_err_t service_ret = ESP_OK;

    if (strcmp(action, "on") == 0) {
        service_ret = valve_service_turn_all_on();
    } else if (strcmp(action, "off") == 0) {
        service_ret = valve_service_turn_all_off();
    } else {
        cJSON_Delete(json);
        return send_error_response(req, "Invalid action. Use 'on' or 'off'", 400);
    }

    cJSON_Delete(json);

    if (service_ret != ESP_OK) {
        return send_error_response(req, "Failed to control all valves", 500);
    }

    // Return updated system status
    valve_system_t *system = valve_service_get_system_status();
    cJSON *response = system_to_json(system);
    return send_json_response(req, response, 200);
}

esp_err_t valve_controller_get_valve_details(httpd_req_t *req)
{
    int valve_id = extract_valve_id_from_uri(req->uri);
    if (valve_id <= 0) {
        return send_error_response(req, "Invalid valve ID", 400);
    }

    ESP_LOGI(TAG, "GET /api/valve/%d", valve_id);

    valve_t *valve = valve_model_get_valve(valve_id);
    if (valve == NULL) {
        return send_error_response(req, "Valve not found", 404);
    }

    cJSON *json = valve_to_json(valve);
    return send_json_response(req, json, 200);
}

esp_err_t valve_controller_update_valve_config(httpd_req_t *req)
{
    int valve_id = extract_valve_id_from_uri(req->uri);
    if (valve_id <= 0) {
        return send_error_response(req, "Invalid valve ID", 400);
    }

    ESP_LOGI(TAG, "PUT /api/valve/%d", valve_id);

    // Read request body
    char buf[500];
    int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
    if (ret <= 0) {
        return send_error_response(req, "Failed to read request body", 400);
    }
    buf[ret] = '\0';

    // Parse JSON
    cJSON *json = cJSON_Parse(buf);
    if (json == NULL) {
        return send_error_response(req, "Invalid JSON", 400);
    }

    // Update valve name if provided
    cJSON *name_json = cJSON_GetObjectItem(json, "name");
    if (cJSON_IsString(name_json)) {
        esp_err_t name_ret = valve_service_set_valve_name(valve_id, name_json->valuestring);
        if (name_ret != ESP_OK) {
            cJSON_Delete(json);
            return send_error_response(req, "Failed to update valve name", 400);
        }
    }

    // Update valve enabled status if provided
    cJSON *enabled_json = cJSON_GetObjectItem(json, "enabled");
    if (cJSON_IsBool(enabled_json)) {
        bool enabled = cJSON_IsTrue(enabled_json);
        esp_err_t enabled_ret = valve_service_enable_valve(valve_id, enabled);
        if (enabled_ret != ESP_OK) {
            cJSON_Delete(json);
            return send_error_response(req, "Failed to update valve enabled status", 400);
        }
    }

    cJSON_Delete(json);

    // Return updated valve status
    valve_t *valve = valve_model_get_valve(valve_id);
    cJSON *response = valve_to_json(valve);
    return send_json_response(req, response, 200);
}

esp_err_t valve_controller_emergency_stop(httpd_req_t *req)
{
    ESP_LOGW(TAG, "POST /api/emergency-stop - EMERGENCY STOP ACTIVATED");

    esp_err_t service_ret = valve_service_emergency_stop();
    if (service_ret != ESP_OK) {
        return send_error_response(req, "Emergency stop failed", 500);
    }

    cJSON *response = cJSON_CreateObject();
    cJSON_AddBoolToObject(response, "success", true);
    cJSON_AddStringToObject(response, "message", "Emergency stop activated - all valves turned off");
    
    return send_json_response(req, response, 200);
}

// Helper function implementations
static esp_err_t send_json_response(httpd_req_t *req, cJSON *json, int status_code)
{
    if (json == NULL) {
        return ESP_FAIL;
    }

    char *json_string = cJSON_Print(json);
    if (json_string == NULL) {
        cJSON_Delete(json);
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_status(req, status_code == 200 ? "200 OK" : 
                         status_code == 400 ? "400 Bad Request" :
                         status_code == 404 ? "404 Not Found" : "500 Internal Server Error");
    
    esp_err_t ret = httpd_resp_sendstr(req, json_string);
    
    free(json_string);
    cJSON_Delete(json);
    return ret;
}

static esp_err_t send_error_response(httpd_req_t *req, const char *error_message, int status_code)
{
    cJSON *json = cJSON_CreateObject();
    cJSON_AddBoolToObject(json, "success", false);
    cJSON_AddStringToObject(json, "error", error_message);
    
    return send_json_response(req, json, status_code);
}

static int extract_valve_id_from_uri(const char *uri)
{
    // Extract valve ID from URI like "/api/valve/1" or "/api/valve/1/control"
    const char *valve_prefix = "/api/valve/";
    const char *valve_start = strstr(uri, valve_prefix);
    if (valve_start == NULL) {
        return -1;
    }
    
    valve_start += strlen(valve_prefix);
    return atoi(valve_start);
}

static cJSON* valve_to_json(valve_t *valve)
{
    if (valve == NULL) {
        return NULL;
    }

    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "id", valve->id);
    cJSON_AddStringToObject(json, "name", valve->name);
    cJSON_AddNumberToObject(json, "gpio_pin", valve->gpio_pin);
    cJSON_AddStringToObject(json, "state", 
        valve->state == VALVE_STATE_ON ? "on" :
        valve->state == VALVE_STATE_OFF ? "off" : "error");
    cJSON_AddStringToObject(json, "mode",
        valve->mode == VALVE_MODE_MANUAL ? "manual" :
        valve->mode == VALVE_MODE_AUTO ? "auto" : "scheduled");
    cJSON_AddNumberToObject(json, "total_runtime_seconds", valve->total_runtime_seconds);
    cJSON_AddNumberToObject(json, "last_activated_timestamp", valve->last_activated_timestamp);
    cJSON_AddBoolToObject(json, "enabled", valve->is_enabled);
    cJSON_AddBoolToObject(json, "inverted", valve->is_inverted);
    
    return json;
}

static cJSON* system_to_json(valve_system_t *system)
{
    if (system == NULL) {
        return NULL;
    }

    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "valve_count", system->valve_count);
    cJSON_AddBoolToObject(json, "system_enabled", system->system_enabled);
    cJSON_AddNumberToObject(json, "system_uptime", system->system_uptime);
    
    cJSON *valves_array = cJSON_CreateArray();
    for (uint8_t i = 0; i < system->valve_count; i++) {
        cJSON *valve_json = valve_to_json(&system->valves[i]);
        if (valve_json != NULL) {
            cJSON_AddItemToArray(valves_array, valve_json);
        }
    }
    cJSON_AddItemToObject(json, "valves", valves_array);
    
    return json;
}
