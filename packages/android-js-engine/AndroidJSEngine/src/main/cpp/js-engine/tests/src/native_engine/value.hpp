#ifndef VALUE_H
#define VALUE_H

#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

class Value {
 public:
  Value(const std::string& input_json);
  ~Value();

  int get_int_value();
  bool get_bool_value();
  float get_float_value();
  std::string get_string_value();

  nlohmann::json get_json_array();
  nlohmann::json get_json_object();

  bool is_null_or_undefined();

 private:
  std::string json_str;

  bool check_if_null_or_undefined();
  double get_js_number();
};

#endif  // VALUE_H