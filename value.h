#pragma once
#include <cstdint>
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include <stdexcept>

namespace caps {

struct Record {
  std::unordered_map<std::string, struct Value> fields;
};

struct Array {
  std::vector<struct Value> elems;
};

struct Tuple {
  std::vector<struct Value> elems;
};

struct Value {
  using V = std::variant<
    std::monostate,        // "unset"
    int64_t,
    bool,
    double,
    std::string,
    Record,
    Array,
    Tuple
  >;

  V v;

  static Value unset() { return Value{std::monostate{}}; }
  static Value i(int64_t x) { return Value{x}; }
  static Value b(bool x) { return Value{x}; }
  static Value r(double x) { return Value{x}; }
  static Value s(std::string x) { return Value{std::move(x)}; }
  static Value rec(Record x) { return Value{std::move(x)}; }
  static Value arr(Array x) { return Value{std::move(x)}; }
  static Value tup(Tuple x) { return Value{std::move(x)}; }

  bool is_unset() const { return std::holds_alternative<std::monostate>(v); }
};

std::string to_string(const Value& x);

bool is_truthy(const Value& x);

// strict typed access (runtime error if mismatch)
int64_t as_int(const Value& x);
bool as_bool(const Value& x);
double as_real(const Value& x);
const std::string& as_text(const Value& x);
const Record& as_record(const Value& x);
Record& as_record(Value& x);
const Array& as_array(const Value& x);
Array& as_array(Value& x);
const Tuple& as_tuple(const Value& x);
Tuple& as_tuple(Value& x);

} // namespace caps
