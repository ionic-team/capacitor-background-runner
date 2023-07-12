cmake_minimum_required(VERSION 3.22.1)

project("android_js_engine")

add_library(
        ${PROJECT_NAME}
        SHARED
        errors.cpp context.cpp runner.cpp native_runner.cpp native_context.cpp api_console.cpp api_crypto.cpp api_events.cpp api_text.cpp api_timeout.cpp api_fetch.cpp)

find_library(
        log-lib
        log)

add_subdirectory("quickjs")

target_link_libraries(${PROJECT_NAME} ${log-lib})
target_link_libraries(${PROJECT_NAME} quickjs)