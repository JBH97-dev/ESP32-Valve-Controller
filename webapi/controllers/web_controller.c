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

    ESP_LOGI(TAG, "Web controller routes registered successfully");
    return ESP_OK;
}

esp_err_t web_controller_serve_index(httpd_req_t *req)
{
    ESP_LOGI(TAG, "GET / - Serving web interface");

    FILE* f = fopen("/spiffs/web/index.html", "r");
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
