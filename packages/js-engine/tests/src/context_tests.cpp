#include <catch2/catch_test_macros.hpp>

#include "native_engine/engine.hpp"

// TEST_CASE("test null eval", "[context]") {
//   std::string context_name = "io.ionic.android_js_engine";
//   auto engine = new Engine();
//   engine->create_context(context_name);

//   auto value = engine->execute(context_name, "undefined");
//   REQUIRE(value->is_null_or_undefined());

//   value = engine->execute(context_name, "const test = null; test;");
//   REQUIRE(value->is_null_or_undefined());

//   delete engine;
// }

// TEST_CASE("test bool eval", "[context]") {
//   std::string context_name = "io.ionic.android_js_engine";
//   auto engine = new Engine();
//   engine->create_context(context_name);

//   auto value = engine->execute(context_name, "let test = (1 == 1); test;");
//   REQUIRE(value->get_bool_value());

//   value = engine->execute(context_name, "test = (100 == 200); test;");
//   REQUIRE(!value->get_bool_value());

//   delete engine;
// }

// TEST_CASE("test integer eval", "[context]") {
//   std::string context_name = "io.ionic.android_js_engine";
//   auto engine = new Engine();
//   engine->create_context(context_name);

//   auto value = engine->execute(context_name, "1 + 2;");
//   REQUIRE(value->get_int_value() == 3);

//   delete engine;
// }

// TEST_CASE("test float eval", "[context]") {
//   std::string context_name = "io.ionic.android_js_engine";
//   auto engine = new Engine();
//   engine->create_context(context_name);

//   auto value = engine->execute(context_name, "10.8 + 2.77;");
//   REQUIRE(value->get_float_value() == 13.57f);

//   delete engine;
// }

// TEST_CASE("test string eval", "[context]") {
//   std::string context_name = "io.ionic.android_js_engine";
//   auto engine = new Engine();
//   engine->create_context(context_name);

//   auto value = engine->execute(context_name, "'hello' + ' ' + 'world';");
//   REQUIRE(value->get_string_value() == "hello world");

//   delete engine;
// }

TEST_CASE("test console api", "[context]") {
  std::string context_name = "io.ionic.android_js_engine";
  auto engine = new Engine();
  engine->create_context(context_name);

  engine->execute(context_name, "console.log('hello world');");
  engine->execute(context_name, "console.info('this message is for informational purposes');");
  engine->execute(context_name, "console.warn('this is a warning message');");
  engine->execute(context_name, "console.error('a problem has occurred');");
  engine->execute(context_name, "console.debug('this is a debugging statement');");

  delete engine;
}