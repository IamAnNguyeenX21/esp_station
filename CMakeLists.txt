# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(EXTRA_COMPONENT_DIRS ${CMAKE_CURRENT_LIST_DIR}/common) # may dong nay phai dat truoc dong project de tranh no such file or directory

project(wifi_station)

target_add_binary_data(${CMAKE_PROJECT_NAME}.elf "html/temp.html" TEXT)