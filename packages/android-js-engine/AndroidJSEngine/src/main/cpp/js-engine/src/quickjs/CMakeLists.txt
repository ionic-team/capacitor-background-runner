cmake_minimum_required(VERSION 3.18.1)

project(quickjs)

# Define the target
add_library(quickjs STATIC
        cutils.c
        libbf.c
        libregexp.c
        libunicode.c
        qjs.c
        qjsc.c
        quickjs.c
        quickjs-libc.c
        unicode_gen.c
        )

add_library(quickjs::quickjs ALIAS quickjs)

# Read the version off the VERSION file
file(READ VERSION CONFIG_VERSION)
string(REGEX REPLACE "\n$" "" CONFIG_VERSION "${CONFIG_VERSION}")

set_target_properties(quickjs PROPERTIES
        VERSION ${CONFIG_VERSION}
        )

# Add #defines and dependencies
target_compile_definitions(quickjs PRIVATE
        CONFIG_VERSION="${CONFIG_VERSION}"
        CONFIG_BIGNUM=1
        )
