cmake_minimum_required(VERSION 3.18.1)

project("android_engine")

add_library(${PROJECT_NAME} SHARED Context.cpp api_console.cpp api_events.cpp api_crypto.cpp android_engine.cpp)

find_library(log-lib log)

add_subdirectory("quickjs")

target_link_libraries(${PROJECT_NAME} ${log-lib})
target_link_libraries(${PROJECT_NAME} quickjs)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall)