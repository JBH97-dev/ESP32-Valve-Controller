idf_component_register(
    SRCS "main.c" "managers/config_manager.c"
    INCLUDE_DIRS "." "managers"
    REQUIRES webapi esp_wifi nvs_flash
)
