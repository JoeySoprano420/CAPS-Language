#pragma once
#include "backend/value.h"

namespace caps {

// Result<T,E> is represented as a Record with fields:
//   ok: bool
//   value: <T> (Value)
//   error: <E> (Value, usually text)
inline Value make_result_ok(Value v) {
  Record r;
  r.fields["ok"] = Value::b(true);
  r.fields["value"] = std::move(v);
  r.fields["error"] = Value::s("");
  return Value::rec(std::move(r));
}

inline Value make_result_err_text(std::string err) {
  Record r;
  r.fields["ok"] = Value::b(false);
  r.fields["value"] = Value::unset();
  r.fields["error"] = Value::s(std::move(err));
  return Value::rec(std::move(r));
}

inline bool result_ok(const Value& rv) {
  auto& rec = as_record(rv);
  return as_bool(rec.fields.at("ok"));
}

inline const Value& result_value(const Value& rv) {
  auto& rec = as_record(rv);
  return rec.fields.at("value");
}

inline const Value& result_error(const Value& rv) {
  auto& rec = as_record(rv);
  return rec.fields.at("error");
}

} // namespace caps
