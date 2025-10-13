idf_component_register(
    SRCS 
        "controllers/valve_controller.c"
        "controllers/web_controller.c"
    INCLUDE_DIRS 
        "."
        "controllers"
    REQUIRES 
        esp_http_server
        
        json
        spiffs
        
        managers
)
