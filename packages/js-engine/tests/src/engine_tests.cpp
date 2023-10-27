#include <fmt/core.h>

#include <catch2/catch_test_macros.hpp>
#include <random>

#include "native_engine/engine.hpp"

int get_random_number() {
  std::random_device rnd_device;
  std::default_random_engine rnd_engine(rnd_device());
  std::uniform_int_distribution<int> uniform_dist(20, 40);

  return uniform_dist(rnd_engine);
}

TEST_CASE("runner create and destroy", "[test]") {
  auto engine = new Engine();
  engine->start();

  engine->stop();
  delete engine;
}

TEST_CASE("runner context create and destroy", "[runner]") {
  auto engine = new Engine();
  engine->start();

  std::string context_name = "io.ionic.android_js_engine";

  engine->create_context(context_name);

  engine->stop();
  delete engine;
}

TEST_CASE("runner create and destroy multiple contexts", "[runner]") {
  auto engine = new Engine();
  engine->start();

  int rnd = get_random_number();

  for (int i = 0; i < rnd; i++) {
    auto context_name = fmt::format("context #{}", i + 1);
    engine->create_context(context_name);
  }

  fmt::println("Created {} contexts", rnd);

  engine->stop();
  delete engine;
}