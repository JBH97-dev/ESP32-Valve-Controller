#include "web_controller.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include <stdio.h>

static const char *TAG = "web_controller";

esp_err_t web_controller_register_routes(httpd_handle_t server)
{
    ESP_LOGI(TAG, "Registering web controller routes");

    // GET / - Serve main web interface
    httpd_uri_t root_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = web_controller_serve_index,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &root_uri);

    // GET /global.css - Serve global.css
    httpd_uri_t global_css_uri = {
        .uri = "/global.css",
        .method = HTTP_GET,
        .handler = web_controller_serve_static,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &global_css_uri);

    // GET /build/bundle.css - Serve build/bundle.css
    httpd_uri_t bundle_css_uri = {
        .uri = "/build/bundle.css",
        .method = HTTP_GET,
        .handler = web_controller_serve_static,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &bundle_css_uri);

    // GET /build/bundle.js - Serve build/bundle.js
    httpd_uri_t bundle_js_uri = {
        .uri = "/build/bundle.js",
        .method = HTTP_GET,
        .handler = web_controller_serve_static,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &bundle_js_uri);

    // GET /build/bundle.js.map - Serve bundle.js.map
    httpd_uri_t bundle_js_map_uri = {
        .uri = "/build/bundle.js.map",
        .method = HTTP_GET,
        .handler = web_controller_serve_static,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &bundle_js_map_uri);

    ESP_LOGI(TAG, "Web controller routes registered successfully");
    return ESP_OK;
}

esp_err_t web_controller_serve_static(httpd_req_t *req)
{
    const char *uri = req->uri;
    ESP_LOGI(TAG, "Serving static file: %s", uri);

    char filepath[256];
    int ret = snprintf(filepath, sizeof(filepath), "/spiffs%s", uri);
    if (ret >= sizeof(filepath)) {
        ESP_LOGE(TAG, "Filepath too long: %s", uri);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    FILE* f = fopen(filepath, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file: %s", filepath);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    char line[512];
    if (strstr(uri, ".css")) {
        httpd_resp_set_type(req, "text/css");
    } else if (strstr(uri, ".js")) {
        httpd_resp_set_type(req, "application/javascript");
    } else {
        httpd_resp_set_type(req, "text/plain");
    }
    
    while (fgets(line, sizeof(line), f)) {
        httpd_resp_sendstr_chunk(req, line);
    }
    
    httpd_resp_sendstr_chunk(req, NULL);
    fclose(f);
    
    ESP_LOGI(TAG, "Static file served successfully: %s", uri);
    return ESP_OK;
}

esp_err_t web_controller_serve_index(httpd_req_t *req)
{
    ESP_LOGI(TAG, "GET / - Serving web interface");

    FILE* f = fopen("/spiffs/index.html", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open index.html");
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    char line[512];
    httpd_resp_set_type(req, "text/html");
    
    while (fgets(line, sizeof(line), f)) {
        httpd_resp_sendstr_chunk(req, line);
    }
    
    httpd_resp_sendstr_chunk(req, NULL);
    fclose(f);
    
    ESP_LOGI(TAG, "Web interface served successfully");
    return ESP_OK;
}
