#include <stdio.h>
#include "esp_log.h"
#include "esp_http_server.h"
#include "managers.h"

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

/* Public function to register the route */
void web_controller_register_routes(httpd_handle_t server,managers_t* managers) {
    httpd_uri_t root_uri = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = root_get_handler,
        .user_ctx  = managers
    };
    httpd_register_uri_handler(server, &root_uri);
}
