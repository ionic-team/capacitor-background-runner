cmake_minimum_required(VERSION 3.22.1)

set(UUID_USING_CXX20_SPAN off)

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.0.1
)

FetchContent_MakeAvailable(Catch2)

find_package(OpenSSL REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(stduuid CONFIG REQUIRED)

add_executable(
  EngineTests
  src/native_engine/engine.cpp
  src/native_engine/value.cpp
  src/native_engine/errors.cpp
  src/native_engine/native_interface.cpp
  src/engine_tests.cpp
)

add_executable(
  ContextTests
  src/native_engine/engine.cpp
  src/native_engine/value.cpp
  src/native_engine/errors.cpp
  src/native_engine/native_interface.cpp
  src/context_tests.cpp
)

target_compile_features(EngineTests PRIVATE cxx_std_20)
target_link_libraries(EngineTests PRIVATE JSEngine)
target_link_libraries(EngineTests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(EngineTests PRIVATE unofficial-sodium::sodium unofficial-sodium::sodium_config_public)
target_link_libraries(EngineTests PRIVATE fmt::fmt)
target_link_libraries(EngineTests PRIVATE stduuid)
target_link_libraries(EngineTests PRIVATE cpr::cpr)
target_link_libraries(EngineTests PRIVATE /opt/homebrew/Cellar/openssl@3/3.1.4/lib/libcrypto.a)
target_link_libraries(EngineTests PRIVATE /opt/homebrew/Cellar/openssl@3/3.1.4/lib/libssl.a)

target_compile_features(ContextTests PRIVATE cxx_std_20)
target_link_libraries(ContextTests PRIVATE JSEngine)
target_link_libraries(ContextTests PRIVATE unofficial-sodium::sodium unofficial-sodium::sodium_config_public)
target_link_libraries(ContextTests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(ContextTests PRIVATE fmt::fmt)
target_link_libraries(ContextTests PRIVATE stduuid)
target_link_libraries(ContextTests PRIVATE cpr::cpr)
target_link_libraries(ContextTests PRIVATE /opt/homebrew/Cellar/openssl@3/3.1.4/lib/libcrypto.a)
target_link_libraries(ContextTests PRIVATE /opt/homebrew/Cellar/openssl@3/3.1.4/lib/libssl.a)

add_test(NAME EngineTests COMMAND EngineTests)
add_test(NAME ContextTests COMMAND ContextTests)