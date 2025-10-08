idf_component_register(
    SRCS "main.c" 
    INCLUDE_DIRS "."
    REQUIRES webapi esp_wifi managers esp_gdbstub
)
