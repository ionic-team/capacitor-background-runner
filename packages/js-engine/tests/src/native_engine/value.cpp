#include "value.hpp"

using json = nlohmann::json;

Value::Value(const std::string& input_json) { this->json_str = input_json; }

int Value::get_int_value() {
  auto val = this->get_js_number();
  if (val == NULL) {
    return NULL;
  }

  return (int)val;
}

float Value::get_float_value() { return this->get_js_number(); }

std::string Value::get_string_value() {
  if (this->check_if_null_or_undefined()) {
    return NULL;
  }

  return this->json_str;
}

bool Value::is_null_or_undefined() { return this->check_if_null_or_undefined(); }

bool Value::get_bool_value() {
  if (this->check_if_null_or_undefined()) {
    return NULL;
  }

  if (this->json_str == "false" || this->json_str == "0" || this->json_str.empty()) {
    return false;
  }

  return true;
}

double Value::get_js_number() {
  if (this->check_if_null_or_undefined()) {
    return NULL;
  }

  return std::stod(this->json_str);
}

bool Value::check_if_null_or_undefined() {
  if (this->json_str == "undefined" || this->json_str == "null") {
    return true;
  }

  return false;
}