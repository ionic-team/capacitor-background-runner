#include <fmt/core.h>

#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <future>
#include <nlohmann/json.hpp>
#include <regex>
#include <stdexcept>
#include <thread>

using json = nlohmann::json;

#include "native_engine/engine.hpp"

TEST_CASE("test null eval", "[context]") {
  std::string context_name = "io.ionic.android_js_engine";
  auto engine = new Engine();
  engine->create_context(context_name);

  auto value = engine->execute(context_name, "undefined");
  REQUIRE(value->is_null_or_undefined());

  value = engine->execute(context_name, "const test = null; test;");
  REQUIRE(value->is_null_or_undefined());

  delete engine;
}

TEST_CASE("test bool eval", "[context]") {
  std::string context_name = "io.ionic.android_js_engine";
  auto engine = new Engine();
  engine->create_context(context_name);

  auto value = engine->execute(context_name, "let test = (1 == 1); test;");
  REQUIRE(value->get_bool_value());

  value = engine->execute(context_name, "test = (100 == 200); test;");
  REQUIRE(!value->get_bool_value());

  delete engine;
}

TEST_CASE("test integer eval", "[context]") {
  std::string context_name = "io.ionic.android_js_engine";
  auto engine = new Engine();
  engine->create_context(context_name);

  auto value = engine->execute(context_name, "1 + 2;");
  REQUIRE(value->get_int_value() == 3);

  delete engine;
}

TEST_CASE("test float eval", "[context]") {
  std::string context_name = "io.ionic.android_js_engine";
  auto engine = new Engine();
  engine->create_context(context_name);

  auto value = engine->execute(context_name, "10.8 + 2.77;");
  REQUIRE(value->get_float_value() == 13.57f);

  delete engine;
}

TEST_CASE("test string eval", "[context]") {
  std::string context_name = "io.ionic.android_js_engine";
  auto engine = new Engine();
  engine->create_context(context_name);

  auto value = engine->execute(context_name, "'hello' + ' ' + 'world';");
  REQUIRE(value->get_string_value() == "\"hello world\"");

  delete engine;
}

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

TEST_CASE("test event listeners", "[context]") {
  auto engine = new Engine();

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

TEST_CASE("test error handling", "[context]") {
  auto engine = new Engine();

  std::string context_name = "io.ionic.android_js_engine";
  engine->create_context(context_name);

  try {
    engine->execute(context_name, "() => { throw new Error('this method has an error'); }();");
    FAIL("error was not caught");
  } catch (JavaScriptException& ex) {
    std::string error_message = ex.what();
    REQUIRE(error_message.find("JS Exception") != std::string::npos);
  } catch (std::exception& ex) {
    FAIL("???");
  }

  try {
    engine->execute(context_name, "addEventListener(");
    FAIL("error was not caught");
  } catch (JavaScriptException& ex) {
    std::string error_message = ex.what();
    REQUIRE(error_message.find("JS Exception") != std::string::npos);
  }

  try {
    engine->execute(context_name, "addEventListener('myThrowingEvent', () => { throw new Error('this event throws an error') })");
    engine->dispatch_event(context_name, "myThrowingEvent", nullptr);
    FAIL("error was not caught");
  } catch (JavaScriptException& ex) {
    std::string error_message = ex.what();
    REQUIRE(error_message.find("JS Exception") != std::string::npos);
  }

  // test handling C++ exceptions
  std::promise<void> future;

  std::function<json(json)> func = [&future](json args) {
    throw std::invalid_argument("a problem occurred in C++");
    return nullptr;
  };

  try {
    engine->register_function(context_name, "exceptionCallback", func);
    engine->execute(context_name, "addEventListener('myThrowingJVMEvent', () => { exceptionCallback(); })");
    engine->dispatch_event(context_name, "myThrowingJVMEvent", nullptr);
    FAIL("error was not caught");
  } catch (JavaScriptException& ex) {
    std::string error_message = ex.what();
    REQUIRE(error_message.find("JS Exception") != std::string::npos);
  }

  delete engine;
}

TEST_CASE("test set timeout", "[context]") {
  auto engine = new Engine();

  std::string context_name = "io.ionic.android_js_engine";
  engine->create_context(context_name);

  std::promise<int> promise1;
  std::promise<int> promise2;

  std::function<json(json)> func_1 = [&promise1](json args) {
    promise1.set_value(1);
    return nullptr;
  };

  std::function<json(json)> func_2 = [&promise2](json args) {
    promise2.set_value(1);
    return nullptr;
  };

  engine->register_function(context_name, "timeoutCallback", func_1);
  engine->register_function(context_name, "cancelTimeoutCallback", func_2);

  auto ret = engine->execute(context_name, "setTimeout(() => { timeoutCallback(); }, 2000)");
  auto timer_id = ret->get_int_value();

  REQUIRE(timer_id > 0);

  auto future1 = promise1.get_future();
  engine->wait_for_jobs();

  auto status = future1.wait_for(std::chrono::seconds(3));
  if (status == std::future_status::timeout) {
    FAIL("timer did not fire");
  }

  REQUIRE(future1.get() == 1);

  ret = engine->execute(context_name, "setTimeout(() => { cancelTimeoutCallback() }, 4000)");
  timer_id = ret->get_int_value();

  REQUIRE(timer_id > 0);

  engine->execute(context_name, fmt::format("clearTimeout({});", timer_id));

  auto future2 = promise2.get_future();
  engine->wait_for_jobs();

  status = future2.wait_for(std::chrono::seconds(5));
  if (status == std::future_status::timeout) {
    SUCCEED("timer did not fire after canceled");
  } else {
    FAIL("timer was not canceled");
  }

  delete engine;
}

TEST_CASE("test set interval", "[context]") {
  bool exit = false;
  auto engine = new Engine();

  auto test_thead = std::thread([engine, &exit]() {
    while (!exit) {
      engine->wait_for_jobs();
    }
  });

  std::string context_name = "io.ionic.android_js_engine";
  engine->create_context(context_name);

  std::promise<int> promise1;
  std::promise<int> promise2;

  int calls = 0;

  std::function<json(json)> func_1 = [&calls](json args) {
    calls++;
    return nullptr;
  };

  engine->register_function(context_name, "intervalCallback", func_1);
  auto ret = engine->execute(context_name, "setInterval(() => { intervalCallback() }, 2000)");
  auto timer_id = ret->get_int_value();

  REQUIRE(timer_id > 0);

  auto future1 = promise1.get_future();

  auto status = future1.wait_for(std::chrono::seconds(8));
  engine->execute(context_name, fmt::format("clearInterval({});", timer_id));

  if (status == std::future_status::timeout) {
    REQUIRE(calls == 4);
  } else {
    FAIL("???");
  }

  auto future2 = promise2.get_future();

  status = future2.wait_for(std::chrono::seconds(3));
  if (status == std::future_status::timeout) {
    REQUIRE(calls == 4);
  } else {
    FAIL("???");
  }

  exit = true;

  test_thead.join();

  delete engine;
}

TEST_CASE("test fetch", "[context]") {
  auto engine = new Engine();

  std::string context_name = "io.ionic.android_js_engine";
  engine->create_context(context_name);

  std::promise<json> success_promise;
  std::promise<json> options_success_promise;
  std::promise<json> failure_promise;
  std::promise<int> binary_promise;

  std::function<json(json)> callback_1 = [&success_promise](json args) {
    success_promise.set_value(args);
    return nullptr;
  };

  std::function<json(json)> callback_2 = [&options_success_promise](json args) {
    options_success_promise.set_value(args);
    return nullptr;
  };

  std::function<json(json)> callback_3 = [&failure_promise](json args) {
    failure_promise.set_value(args);
    return nullptr;
  };

  std::function<json(json)> callback_4 = [&binary_promise](json args) {
    try {
      auto size = args["size"].template get<int>();
      binary_promise.set_value(size);
    } catch (std::exception& ex) {
      binary_promise.set_value(-1);
    }

    return nullptr;
  };

  engine->register_function(context_name, "successCallback", callback_1);
  engine->register_function(context_name, "successCallback2", callback_2);
  engine->register_function(context_name, "failureCallback", callback_3);
  engine->register_function(context_name, "successCallback3", callback_4);

  std::string basic_fetch_example =
      "fetch('https://jsonplaceholder.typicode.com/todos/1')"
      ".then(response => response.json())"
      ".then(json => { successCallback(json); })"
      ".catch(err => { console.error(err); });";

  engine->execute(context_name, basic_fetch_example);
  engine->wait_for_jobs();

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
  engine->wait_for_jobs();

  auto future2 = options_success_promise.get_future();
  status = future2.wait_for(std::chrono::seconds(5));
  if (status == std::future_status::timeout) {
    FAIL("could not get request");
  }

  response_data = future2.get();
  auto body = response_data["body"].template get<std::string>();
  REQUIRE(body == "bar");

  std::string fetch_failure_example =
      "fetch('https://blablabla.fake/todos/1')"
      ".catch(err => { console.error(err);  failureCallback(err); });";

  engine->execute(context_name, fetch_failure_example);
  engine->wait_for_jobs();

  auto future3 = failure_promise.get_future();
  status = future3.wait_for(std::chrono::seconds(3));
  if (status == std::future_status::timeout) {
    FAIL("request failure was not caught");
  }

  response_data = future3.get();
  SUCCEED(response_data.dump());

  std::string fetch_image_example =
      "fetch('https://placehold.co/600x400@3x.png')"
      ".then((res) =>  { return res.blob(); })"
      ".then((blob) => { successCallback3({ size: blob.size }); })";

  engine->execute(context_name, fetch_image_example);
  engine->wait_for_jobs();

  auto future4 = binary_promise.get_future();
  status = future4.wait_for(std::chrono::seconds(3));
  if (status == std::future_status::timeout) {
    FAIL("request failure was not caught");
  }

  auto image_size = future4.get();
  REQUIRE(image_size == 62404);

  delete engine;
}

TEST_CASE("test crypto", "[context]") {
  auto engine = new Engine();

  std::string context_name = "io.ionic.android_js_engine";
  engine->create_context(context_name);

  auto value = engine->execute(context_name, "const array = new Uint32Array(10);  crypto.getRandomValues(array); array;");
  auto json_object = value->get_json_object();
  auto obj = json_object.get<std::unordered_map<std::string, int>>();

  REQUIRE(obj.size() == 10);

  value = engine->execute(context_name, "crypto.randomUUID();");
  auto random_string = std::regex_replace(value->get_string_value(), std::regex("^\"+|\"+$|( ) +"), "$1");
  REQUIRE(random_string.length() == 36);

  delete engine;
}

TEST_CASE("test text encoder", "[context]") {
  auto engine = new Engine();

  std::string context_name = "io.ionic.android_js_engine";
  engine->create_context(context_name);

  auto value = engine->execute(context_name, "const encoder = new TextEncoder(); encoder.encode('€');");
  auto json_object = value->get_json_object();
  auto obj = json_object.get<std::unordered_map<std::string, int>>();

  REQUIRE(obj["0"] == 226);
  REQUIRE(obj["1"] == 130);
  REQUIRE(obj["2"] == 172);

  delete engine;
}

TEST_CASE("test text decoder", "[context]") {
  auto engine = new Engine();

  std::string context_name = "io.ionic.android_js_engine";
  engine->create_context(context_name);

  auto value = engine->execute(context_name, "const win1251decoder = new TextDecoder(\"windows-1251\"); win1251decoder.decode(new Uint8Array([ 207, 240, 232, 226, 229, 242, 44, 32, 236, 232, 240, 33]));");
  REQUIRE(value->get_string_value() == "Привет, мир!");

  // value = engine->execute(context_name, "const decoder = new TextDecoder(); decoder.decode(new Uint8Array([240, 160, 174, 183]));");
  // REQUIRE(value->get_string_value() == "\UD842\UDFB7");

  delete engine;
}

TEST_CASE("blob tests", "[context]") {
  auto engine = new Engine();

  std::string context_name = "io.ionic.android_js_engine";
  engine->create_context(context_name);

  std::promise<int> array_buffer_promise;
  std::promise<std::string> text_promise;

  std::function<json(json)> callback_1 = [&array_buffer_promise](json args) {
    auto size = args["size"].template get<int>();
    array_buffer_promise.set_value(size);
    return nullptr;
  };

  std::function<json(json)> callback_2 = [&text_promise](json args) {
    auto text = args.template get<std::string>();
    text_promise.set_value(text);
    return nullptr;
  };

  engine->register_function(context_name, "arrayBufferCallback", callback_1);
  engine->register_function(context_name, "textCallback", callback_2);

  std::string basic_blob_example =
      "const testString = \"testing\";"
      "const testArrayBuf = new ArrayBuffer(32);"
      "const testTypedArr = new Int8Array(16);"
      "const blob = new Blob([testString, testArrayBuf, testTypedArr]);";

  engine->execute(context_name, basic_blob_example);

  std::string get_blob_array_buffer_example = "blob.arrayBuffer().then((buffer) => { arrayBufferCallback({size: buffer.byteLength}); });";
  engine->execute(context_name, get_blob_array_buffer_example);
  engine->wait_for_jobs();

  auto array_buffer_future = array_buffer_promise.get_future();
  array_buffer_future.wait();

  auto array_buffer_data = array_buffer_future.get();
  REQUIRE(array_buffer_data == 55);

  std::string get_blob_text_example = "blob.text().then((text) => { textCallback(text); });";

  engine->execute(context_name, get_blob_text_example);
  engine->wait_for_jobs();

  auto text_future = text_promise.get_future();
  text_future.wait();

  auto text = text_future.get();
  REQUIRE(text == "testing");

  delete engine;
}

TEST_CASE("headers tests", "[context]") {
  auto engine = new Engine();

  std::string context_name = "io.ionic.android_js_engine";
  engine->create_context(context_name);

  std::string basic_headers_example = "const h = new Headers()";

  engine->execute(context_name, basic_headers_example);

  std::string headers_set_example = "h.set(\"Content-Type\", \"application/json\");";
  engine->execute(context_name, headers_set_example);

  std::string headers_get_example = "h.get(\"Content-Type\");";
  auto header_value = engine->execute(context_name, headers_get_example);

  REQUIRE(header_value->get_string_value() == "\"application/json\"");

  std::string headers_has_example = "h.has(\"Content-Type\");";
  auto has_value = engine->execute(context_name, headers_has_example);

  REQUIRE(has_value->get_bool_value());

  std::string headers_delete_example = "h.delete(\"Content-Type\");";
  engine->execute(context_name, headers_delete_example);

  auto deleted_value = engine->execute(context_name, headers_get_example);

  REQUIRE(deleted_value->is_null_or_undefined());

  has_value = engine->execute(context_name, headers_has_example);

  REQUIRE(has_value->get_bool_value() == false);

  std::string headers_append_example =
      "h.set(\"Accept\", \"text/html\");"
      "h.append(\"Accept\", \"application/xhtml+xml\");";

  engine->execute(context_name, headers_append_example);

  auto appended_value = engine->execute(context_name, "h.get(\"Accept\");");
  REQUIRE(appended_value->get_string_value() == "\"text/html, application/xhtml+xml\"");

  engine->execute(context_name, headers_set_example);

  std::string headers_all_keys = "for(const key of h.keys()) { console.log(key); }";
  engine->execute(context_name, headers_all_keys);

  std::string headers_all_values = "for(const value of h.values()) { console.log(value); }";
  engine->execute(context_name, headers_all_values);

  std::string headers_all_entries = "for(const pair of h.entries()) { console.log(`${pair[0]}: ${pair[1]}`); }";
  engine->execute(context_name, headers_all_entries);

  delete engine;
}