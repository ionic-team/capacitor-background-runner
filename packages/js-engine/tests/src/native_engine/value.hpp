#ifndef VALUE_H
#define VALUE_H

#include <nlohmann/json.hpp>
#include <string>

class Value {
 public:
  Value(const std::string& input_json);
  ~Value();

  int get_int_value();
  bool get_bool_value();
  float get_float_value();
  std::string get_string_value();

  bool is_null_or_undefined();

 private:
  nlohmann::json json_obj;
  std::string json_str;

  bool check_if_null_or_undefined();
  double get_js_number();
};

#endif  // VALUE_H