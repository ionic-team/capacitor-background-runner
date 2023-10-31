cmake_minimum_required(VERSION 3.22.1)

project("JSEngine")

add_library(
    ${PROJECT_NAME}
    SHARED
    src/context.cpp
    src/runner.cpp
    src/api/api_console.cpp
    src/api/api_events.cpp
    src/api/api_timeout.cpp
)

add_subdirectory("src/quickjs")

target_link_libraries(${PROJECT_NAME} quickjs)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

enable_testing()
add_subdirectory("tests")
