cmake_minimum_required(VERSION 3.5)

set(COMPONENTS "main" "esp32" "esptool_py" "esp_gdbstub")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

#add_custom_target(archive ALL tar -cf archive.tar)

project(rleded)
