#include <stdio.h>
#include "esp_log.h"
#include "esp_http_server.h"
#include "managers.h"
#include "esp_spiffs.h"
#include <sys/stat.h>

#define WEB_STATIC_PATH "/spiffs/web/static"

static const char *TAG = "web_controller";

/* GET / handler */
esp_err_t root_get_handler(httpd_req_t *req) {
    FILE *f = fopen("/spiffs/web/index.html", "r");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open /spiffs/web/index.html");
        httpd_resp_send_404(req);
        return ESP_OK;
    }

    httpd_resp_set_type(req, "text/html");

    char buffer[512];
    size_t read_bytes;
    while ((read_bytes = fread(buffer, 1, sizeof(buffer), f)) > 0) {
        httpd_resp_send_chunk(req, buffer, read_bytes);
    }
    fclose(f);

    httpd_resp_send_chunk(req, NULL, 0); // End response
    return ESP_OK;
}


esp_err_t web_controller_static_handler(httpd_req_t *req)
{
    char filepath[128];
    const char *uri = req->uri;

    // Extract path after "/static"
    const char *static_path = uri + strlen("/static");
    if (strlen(static_path) == 0) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "No file specified");
        return ESP_FAIL;
    }

    snprintf(filepath, sizeof(filepath), "%s%s", WEB_STATIC_PATH, static_path);

    FILE *file = fopen(filepath, "rb");
    if (!file) {
        ESP_LOGW("web_static", "File not found: %s", filepath);
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File not found");
        return ESP_FAIL;
    }

    // Set content type based on file extension (basic)
    if (strstr(filepath, ".css")) {
        httpd_resp_set_type(req, "text/css");
    } else if (strstr(filepath, ".js")) {
        httpd_resp_set_type(req, "application/javascript");
    } else if (strstr(filepath, ".png")) {
        httpd_resp_set_type(req, "image/png");
    } else if (strstr(filepath, ".jpg") || strstr(filepath, ".jpeg")) {
        httpd_resp_set_type(req, "image/jpeg");
    } else {
        httpd_resp_set_type(req, "application/octet-stream");
    }

    char chunk[1024];
    size_t read_bytes;
    while ((read_bytes = fread(chunk, 1, sizeof(chunk), file)) > 0) {
        if (httpd_resp_send_chunk(req, chunk, read_bytes) != ESP_OK) {
            fclose(file);
            httpd_resp_sendstr_chunk(req, NULL); // End response
            return ESP_FAIL;
        }
    }
    fclose(file);
    httpd_resp_sendstr_chunk(req, NULL); // End response
    return ESP_OK;
}

/* Public function to register the route */
void web_controller_register_routes(httpd_handle_t server, managers_t *managers)
{
    httpd_uri_t root_uri = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = root_get_handler,
        .user_ctx  = managers
    };
    httpd_register_uri_handler(server, &root_uri);

    // Static file handler
    httpd_uri_t static_uri = {
        .uri      = "/static/*",
        .method   = HTTP_GET,
        .handler  = web_controller_static_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &static_uri);
}
