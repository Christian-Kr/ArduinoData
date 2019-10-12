cmake_minimum_required(VERSION 3.13.4)

project(ArduinoData)

include_directories("${PROJECT_SOURCE_DIR}/src")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "${PROJECT_SOURCE_DIR}/ui")

find_package(Qt5 COMPONENTS Widgets SerialPort Charts REQUIRED)

add_subdirectory(src)
