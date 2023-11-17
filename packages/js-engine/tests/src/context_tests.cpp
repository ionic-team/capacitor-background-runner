#include <fmt/core.h>

#include <catch2/catch_test_macros.hpp>
#include <chrono>
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

// TEST_CASE("test event listeners", "[context]") {
//   std::promise<int> future1;
//   std::promise<int> future2;
//   std::promise<json> future3;
//   std::promise<int> future4;

//   std::function<json(json)> func_1 = [&future1](json args) {
//     future1.set_value(1);
//     return nullptr;
//   };

//   std::function<json(json)> func_2 = [&future2](json args) {
//     future2.set_value(1);
//     return nullptr;
//   };

//   std::function<json(json)> func_3 = [&future3](json args) {
//     future3.set_value(args);
//     return nullptr;
//   };

//   std::function<json(json)> func_4 = [&future4](json args) {
//     future4.set_value(1);
//     return nullptr;
//   };

//   std::string context_name = "io.ionic.android_js_engine";
//   auto engine = new Engine();
//   engine->create_context(context_name);
//   engine->register_function(context_name, "successCallback", func_1);
//   engine->register_function(context_name, "altSuccessCallback", func_2);
//   engine->register_function(context_name, "successCallbackDetails", func_3);
//   engine->register_function(context_name, "successCallbackFunction", func_4);

//   // setting a basic event listener
//   engine->execute(context_name, "addEventListener('myEvent', () => { successCallback(); });");
//   engine->dispatch_event(context_name, "myEvent", nullptr);

//   REQUIRE(future1.get_future().get() == 1);

//   engine->execute(context_name, "addEventListener('myEvent', () => { altSuccessCallback(); });");
//   engine->dispatch_event(context_name, "myEvent", nullptr);

//   REQUIRE(future2.get_future().get() == 1);

//   // basic event listener with details
//   engine->execute(context_name, "addEventListener('myEventDetails', (details) => { successCallbackDetails(details); });");

//   json details;
//   details["name"] = "John Doe";

//   engine->dispatch_event(context_name, "myEventDetails", details);

//   auto value = future3.get_future().get();

//   REQUIRE(value["name"] == "John Doe");

//   // basic event listener with registered global function as callback arg
//   engine->execute(context_name, "addEventListener('myEventCallback', (resolve) => { resolve() });");

//   json args;
//   json callbacks;

//   callbacks["resolve"] = "__cbr::successCallbackFunction";
//   args["callbacks"] = callbacks;

//   engine->dispatch_event(context_name, "myEventCallback", args);

//   REQUIRE(future4.get_future().get() == 1);

//   delete engine;
// }

// TEST_CASE("test error handling", "[context]") {
//   std::string context_name = "io.ionic.android_js_engine";
//   auto engine = new Engine();
//   engine->create_context(context_name);

//   try {
//     engine->execute(context_name, "() => { throw new Error('this method has an error'); }();");
//     REQUIRE(false);
//   } catch (JavaScriptException& ex) {
//     std::string error_message = ex.what();
//     REQUIRE(error_message.find("JS Exception") != std::string::npos);
//   }

//   try {
//     engine->execute(context_name, "addEventListener(");
//     REQUIRE(false);
//   } catch (JavaScriptException& ex) {
//     std::string error_message = ex.what();
//     REQUIRE(error_message.find("JS Exception") != std::string::npos);
//   }

//   try {
//     engine->execute(context_name, "addEventListener('myThrowingEvent', () => { throw new Error('this event throws an error') })");
//     engine->dispatch_event(context_name, "myThrowingEvent", nullptr);
//   } catch (JavaScriptException& ex) {
//     std::string error_message = ex.what();
//     REQUIRE(error_message.find("JS Exception") != std::string::npos);
//   }

//   // test handling C++ exceptions
//   std::promise<void> future;

//   std::function<json(json)> func = [&future](json args) {
//     throw std::invalid_argument("a problem occurred in C++");
//     return nullptr;
//   };

//   try {
//     engine->register_function(context_name, "exceptionCallback", func);
//     engine->execute(context_name, "addEventListener('myThrowingJVMEvent', () => { exceptionCallback(); })");
//     engine->dispatch_event(context_name, "myThrowingJVMEvent", nullptr);
//   } catch (JavaScriptException& ex) {
//     std::string error_message = ex.what();
//     REQUIRE(error_message.find("JS Exception") != std::string::npos);
//   }
// }

// TEST_CASE("test set timeout", "[context]") {
//   std::string context_name = "io.ionic.android_js_engine";
//   auto engine = new Engine();
//   engine->start();

//   engine->create_context(context_name);

//   std::promise<int> promise1;
//   std::promise<int> promise2;

//   std::function<json(json)> func_1 = [&promise1](json args) {
//     promise1.set_value(1);
//     return nullptr;
//   };

//   std::function<json(json)> func_2 = [&promise2](json args) {
//     promise2.set_value(1);
//     return nullptr;
//   };

//   engine->register_function(context_name, "timeoutCallback", func_1);
//   engine->register_function(context_name, "cancelTimeoutCallback", func_2);

//   auto ret = engine->execute(context_name, "setTimeout(() => { timeoutCallback(); }, 2000)");
//   auto timer_id = ret->get_int_value();

//   REQUIRE(timer_id > 0);

//   auto future1 = promise1.get_future();

//   auto status = future1.wait_for(std::chrono::seconds(3));
//   if (status == std::future_status::timeout) {
//     FAIL("timer did not fire");
//   }

//   REQUIRE(future1.get() == 1);

//   ret = engine->execute(context_name, "setTimeout(() => { cancelTimeoutCallback() }, 4000)");
//   timer_id = ret->get_int_value();

//   REQUIRE(timer_id > 0);

//   engine->execute(context_name, fmt::format("clearTimeout({});", timer_id));

//   auto future2 = promise2.get_future();

//   status = future2.wait_for(std::chrono::seconds(5));
//   if (status == std::future_status::timeout) {
//     SUCCEED("timer did not fire after canceled");
//   } else {
//     FAIL("timer was not canceled");
//   }

//   engine->stop();
//   delete engine;
// }

// TEST_CASE("test set interval", "[context]") {
//   std::string context_name = "io.ionic.android_js_engine";
//   auto engine = new Engine();
//   engine->start();

//   engine->create_context(context_name);

//   std::promise<int> promise1;
//   std::promise<int> promise2;

//   int calls = 0;

//   std::function<json(json)> func_1 = [&calls](json args) {
//     calls++;
//     return nullptr;
//   };

//   engine->register_function(context_name, "intervalCallback", func_1);
//   auto ret = engine->execute(context_name, "setInterval(() => { intervalCallback() }, 2000)");
//   auto timer_id = ret->get_int_value();

//   REQUIRE(timer_id > 0);

//   auto future1 = promise1.get_future();
//   auto status = future1.wait_for(std::chrono::seconds(8));
//   if (status == std::future_status::timeout) {
//     REQUIRE(calls == 4);
//   } else {
//     FAIL("???");
//   }

//   engine->execute(context_name, fmt::format("clearInterval({});", timer_id));

//   auto future2 = promise2.get_future();
//   status = future2.wait_for(std::chrono::seconds(3));
//   if (status == std::future_status::timeout) {
//     REQUIRE(calls == 4);
//   } else {
//     FAIL("???");
//   }
//   engine->stop();
//   delete engine;
// }

TEST_CASE("test fetch", "[context]") {
  auto engine = new Engine();

  auto test_thread = std::thread([engine] {
    std::string context_name = "io.ionic.android_js_engine";
    engine->create_context(context_name);

    std::promise<json> success_promise;
    std::promise<json> options_success_promise;

    std::function<json(json)> callback_1 = [&success_promise](json args) {
      success_promise.set_value(args);
      return nullptr;
    };

    std::function<json(json)> callback_2 = [&options_success_promise](json args) {
      options_success_promise.set_value(args);
      return nullptr;
    };

    engine->register_function(context_name, "successCallback", callback_1);
    engine->register_function(context_name, "successCallback2", callback_2);

    std::string basic_fetch_example =
        "fetch('https://jsonplaceholder.typicode.com/todos/1')"
        ".then(response => response.json())"
        ".then(json => { successCallback(json); })"
        ".catch(err => { console.error(err); });";

    engine->execute(context_name, basic_fetch_example);

    auto future1 = success_promise.get_future();
    auto status = future1.wait_for(std::chrono::seconds(5));
    if (status == std::future_status::timeout) {
      FAIL("could not get request");
    }

    auto response_data = future1.get();
    auto title = response_data["title"].template get<std::string>();
    REQUIRE(title == "delectus aut autem");

    std::string basic_fetch_options_example =
        "fetch('https://jsonplaceholder.typicode.com/posts', {"
        "method: 'POST',"
        "body: JSON.stringify({"
        "title: 'foo',"
        "body: 'bar',"
        "userId: 1,"
        "}),"
        "headers: {"
        "'Content-type': 'application/json; charset=UTF-8',"
        "}"
        "})"
        ".catch(err => { console.error(err); })"
        ".then(response => response.json())"
        ".then(json => { successCallback2(json); })";

    engine->execute(context_name, basic_fetch_options_example);

    auto future2 = options_success_promise.get_future();
    status = future2.wait_for(std::chrono::seconds(5));
    if (status == std::future_status::timeout) {
      FAIL("could not get request");
    }

    response_data = future2.get();
    auto body = response_data["body"].template get<std::string>();
    REQUIRE(body == "bar");

    engine->stop();
  });

  engine->start();
  test_thread.join();

  delete engine;
}