cmake_minimum_required(VERSION 3.17)
project(Code)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

include_directories(.)

add_executable(Code
        main.cpp
        mainview.cpp
        mainview.h
        mainwindow.cpp
        mainwindow.h
        model.cpp
        model.h
        user_input.cpp
        utility.cpp
        vertex.h
        shaders/normal_fragshader.glsl
        shaders/normal_vertshader.glsl
        shaders/phong_fragshader.glsl
        shaders/phong_vertshader.glsl
        shaders/gouraud_fragshader.glsl
        shaders/gouraud_vertshader.glsl
        shaders/zbuffer_fragshader.glsl
        shaders/zbuffer_vertshader.glsl
        resources.qrc
        )

target_link_libraries(Code Qt5::Widgets)
target_link_libraries(Code Qt5::Core)