if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

add_executable(${PROJECT_NAME}
    main.cpp
    arduinodata.cpp
    plot.cpp
)

target_link_libraries(${PROJECT_NAME} 
    Qt5::Widgets 
    Qt5::SerialPort
    Qt5::Charts
)
