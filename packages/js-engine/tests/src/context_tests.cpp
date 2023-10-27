#include <fmt/core.h>

#include <catch2/catch_test_macros.hpp>
#include <future>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

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

// TEST_CASE("test console api", "[context]") {
//   std::string context_name = "io.ionic.android_js_engine";
//   auto engine = new Engine();
//   engine->create_context(context_name);

//   engine->execute(context_name, "console.log('hello world');");
//   engine->execute(context_name, "console.info('this message is for informational purposes');");
//   engine->execute(context_name, "console.warn('this is a warning message');");
//   engine->execute(context_name, "console.error('a problem has occurred');");
//   engine->execute(context_name, "console.debug('this is a debugging statement');");

//   delete engine;
// }

TEST_CASE("test event listeners", "[context]") {
  std::promise<int> future1;
  std::promise<int> future2;
  std::promise<json> future3;
  std::promise<int> future4;

  std::function<json(json)> func_1 = [&future1](json args) {
    future1.set_value(1);
    return nullptr;
  };

  std::function<json(json)> func_2 = [&future2](json args) {
    future2.set_value(1);
    return nullptr;
  };

  std::function<json(json)> func_3 = [&future3](json args) {
    future3.set_value(args);
    return nullptr;
  };

  std::function<json(json)> func_4 = [&future4](json args) {
    future4.set_value(1);
    return nullptr;
  };

  std::string context_name = "io.ionic.android_js_engine";
  auto engine = new Engine();
  engine->create_context(context_name);
  engine->register_function(context_name, "successCallback", func_1);
  engine->register_function(context_name, "altSuccessCallback", func_2);
  engine->register_function(context_name, "successCallbackDetails", func_3);
  engine->register_function(context_name, "successCallbackFunction", func_4);

  // setting a basic event listener
  engine->execute(context_name, "addEventListener('myEvent', () => { successCallback(); });");
  engine->dispatch_event(context_name, "myEvent", nullptr);

  REQUIRE(future1.get_future().get() == 1);

  engine->execute(context_name, "addEventListener('myEvent', () => { altSuccessCallback(); });");
  engine->dispatch_event(context_name, "myEvent", nullptr);

  REQUIRE(future2.get_future().get() == 1);

  // basic event listener with details
  engine->execute(context_name, "addEventListener('myEventDetails', (details) => { successCallbackDetails(details); });");

  json details;
  details["name"] = "John Doe";

  engine->dispatch_event(context_name, "myEventDetails", details);

  auto value = future3.get_future().get();

  REQUIRE(value["name"] == "John Doe");

  // basic event listener with registered global function as callback arg
  engine->execute(context_name, "addEventListener('myEventCallback', (resolve) => { resolve() });");

  json args;
  json callbacks;

  callbacks["resolve"] = "__cbr::successCallbackFunction";
  args["callbacks"] = callbacks;

  engine->dispatch_event(context_name, "myEventCallback", args);

  REQUIRE(future4.get_future().get() == 1);

  delete engine;
}