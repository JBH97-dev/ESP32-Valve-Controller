# The following lines of boilerplate have to be in your project's
# CMakeLists.txt file.

cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS webapi)

set(PARTITION_TABLE_CSV file("partitions.csv"))

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(valve_controller)

# Run JS script before build
execute_process(
    COMMAND node ${CMAKE_SOURCE_DIR}/scripts/copy_web_build_to_spiffs.js
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)


#copying web app artifact to /spiffs partition
spiffs_create_partition_image(spiffs  "${CMAKE_SOURCE_DIR}/spiffs" FLASH_IN_PROJECT)