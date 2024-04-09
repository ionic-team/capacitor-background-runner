cmake_minimum_required(VERSION 3.22.1)

project("android_js_engine")

add_library(
        ${CMAKE_PROJECT_NAME} SHARED
        android_js_engine.cpp
        native_runner.cpp
        native_context.cpp
        native_android_interface.cpp
        native_capacitor_interface.cpp
        java.cpp
        java_errors.cpp
)

add_subdirectory("./js-engine")

target_link_libraries(${CMAKE_PROJECT_NAME} android log JSEngine)