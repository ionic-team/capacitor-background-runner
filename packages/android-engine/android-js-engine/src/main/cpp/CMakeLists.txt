cmake_minimum_required(VERSION 3.22.1)

project("android_js_engine")

add_library(
        ${PROJECT_NAME}
        SHARED
        native_runner.cpp native_context.cpp

        runner.cpp context.cpp errors.cpp java.cpp

        api/api_console.cpp
        api/api_events.cpp
        api/api_timeout.cpp
        api/api_crypto.cpp
        api/api_text.cpp
        api/api_fetch.cpp
        api/api_js_response.cpp
        api/api_blob.cpp

        cap_api/api_cap_kv.cpp
        cap_api/api_cap_device.cpp
        cap_api/api_cap_geolocation.cpp
        cap_api/api_cap_notifications.cpp
)

find_library(
        log-lib
        log)

add_subdirectory("quickjs")

target_link_libraries(${PROJECT_NAME} ${log-lib})
target_link_libraries(${PROJECT_NAME} quickjs)