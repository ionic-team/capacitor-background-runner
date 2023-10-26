cmake_minimum_required(VERSION 3.22.1)

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.0.1
)

FetchContent_MakeAvailable(Catch2)

find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(
  EngineTests
  src/native_engine/engine.cpp
  src/native_engine/value.cpp
  src/test_native.cpp
  src/engine_tests.cpp
)

add_executable(
  ContextTests
  src/native_engine/engine.cpp
  src/native_engine/value.cpp
  src/test_native.cpp
  src/context_tests.cpp
)

target_link_libraries(EngineTests PRIVATE JSEngine)
target_link_libraries(EngineTests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(EngineTests PRIVATE fmt::fmt)

target_compile_features(ContextTests PRIVATE cxx_std_17)
target_link_libraries(ContextTests PRIVATE JSEngine)
target_link_libraries(ContextTests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(ContextTests PRIVATE fmt::fmt)

add_test(NAME EngineTests COMMAND EngineTests)
add_test(NAME ContextTests COMMAND ContextTests)