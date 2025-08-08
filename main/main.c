#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_http_server.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

// Include our web API components
#include "../webapi/services/valve_service.h"
#include "../webapi/controllers/valve_controller.h"
#include "../webapi/controllers/web_controller.h"

// WiFi Configuration - Access Point Mode
#define WIFI_AP_SSID      "ESP32-Valve-Controller"
#define WIFI_AP_PASS      "valve123"
#define WIFI_AP_CHANNEL   1
#define WIFI_AP_MAX_CONN  4

// Application Configuration
#define VALVE_COUNT 4

// Global variables
static const char *TAG = "main";
static httpd_handle_t server = NULL;

// Function declarations
static esp_err_t init_spiffs(void);
static httpd_handle_t start_webserver(void);
static void stop_webserver(httpd_handle_t server);
static void wifi_init_ap(void);

void app_main(void)
{
    ESP_LOGI(TAG, "Starting ESP32 Valve Controller in Access Point mode");

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize SPIFFS
    ESP_ERROR_CHECK(init_spiffs());

    DIR* dir = opendir("/spiffs");
    if (dir == NULL) {
        ESP_LOGE(TAG, "Failed to open directory");
        return;
    }

    struct dirent *de;
    while ((de = readdir(dir)) != NULL) {
        ESP_LOGI(TAG, "Found file: %s", de->d_name);
    }

    closedir(dir);

    // Initialize valve service (business logic)
    ESP_ERROR_CHECK(valve_service_init(VALVE_COUNT));

    // Initialize WiFi Access Point
    wifi_init_ap();

    ESP_LOGI(TAG, "ESP32 Valve Controller started successfully");
    ESP_LOGI(TAG, "=== CONNECTION INSTRUCTIONS ===");
    ESP_LOGI(TAG, "1. Connect to WiFi network: %s", WIFI_AP_SSID);
    ESP_LOGI(TAG, "2. Use password: %s", WIFI_AP_PASS);
    ESP_LOGI(TAG, "3. Open browser to: http://192.168.4.1");
    ESP_LOGI(TAG, "===============================");
}

static void wifi_init_ap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = WIFI_AP_SSID,
            .ssid_len = strlen(WIFI_AP_SSID),
            .channel = WIFI_AP_CHANNEL,
            .password = WIFI_AP_PASS,
            .max_connection = WIFI_AP_MAX_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    
    if (strlen(WIFI_AP_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi AP started. SSID:%s password:%s channel:%d",
             WIFI_AP_SSID, WIFI_AP_PASS, WIFI_AP_CHANNEL);
    
    // Start web server immediately since AP is ready
    server = start_webserver();
    if (server) {
        ESP_LOGI(TAG, "Web server started successfully");
        ESP_LOGI(TAG, "Connect to WiFi: %s", WIFI_AP_SSID);
        ESP_LOGI(TAG, "Password: %s", WIFI_AP_PASS);
        ESP_LOGI(TAG, "Open browser to: http://192.168.4.1");
    }
}

static esp_err_t init_spiffs(void)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ret;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    return ESP_OK;
}

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;
    config.max_uri_handlers = 16;

    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI(TAG, "Registering URI handlers");
        
        // Register web controller routes (serves HTML interface)
        web_controller_register_routes(server);
        
        // Register valve controller routes (API endpoints)
        valve_controller_register_routes(server);

        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

static void stop_webserver(httpd_handle_t server)
{
    httpd_stop(server);
}
