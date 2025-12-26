#include "backend/value.h"
#include <sstream>

namespace caps {

static std::string esc(const std::string& s) {
  std::string out;
  out.reserve(s.size()+8);
  for (char c : s) {
    if (c == '\\') out += "\\\\";
    else if (c == '"') out += "\\\"";
    else if (c == '\n') out += "\\n";
    else out += c;
  }
  return out;
}

std::string to_string(const Value& x) {
  std::ostringstream os;

  if (std::holds_alternative<std::monostate>(x.v)) return "(unset)";
  if (auto p = std::get_if<int64_t>(&x.v)) { os << *p; return os.str(); }
  if (auto p = std::get_if<bool>(&x.v)) { os << (*p ? "true":"false"); return os.str(); }
  if (auto p = std::get_if<double>(&x.v)) { os << *p; return os.str(); }
  if (auto p = std::get_if<std::string>(&x.v)) { os << "\"" << esc(*p) << "\""; return os.str(); }

  if (auto p = std::get_if<Record>(&x.v)) {
    os << "{";
    bool first = true;
    for (auto& kv : p->fields) {
      if (!first) os << ", ";
      first = false;
      os << kv.first << "=" << to_string(kv.second);
    }
    os << "}";
    return os.str();
  }

  if (auto p = std::get_if<Array>(&x.v)) {
    os << "[";
    for (size_t i=0;i<p->elems.size();i++){
      if (i) os << ", ";
      os << to_string(p->elems[i]);
    }
    os << "]";
    return os.str();
  }

  if (auto p = std::get_if<Tuple>(&x.v)) {
    os << "(";
    for (size_t i=0;i<p->elems.size();i++){
      if (i) os << ", ";
      os << to_string(p->elems[i]);
    }
    os << ")";
    return os.str();
  }

  return "(unknown)";
}

bool is_truthy(const Value& x) {
  if (auto p = std::get_if<bool>(&x.v)) return *p;
  throw std::runtime_error("truthiness requires bool");
}

int64_t as_int(const Value& x) {
  if (auto p = std::get_if<int64_t>(&x.v)) return *p;
  throw std::runtime_error("expected int");
}

bool as_bool(const Value& x) {
  if (auto p = std::get_if<bool>(&x.v)) return *p;
  throw std::runtime_error("expected bool");
}

double as_real(const Value& x) {
  if (auto p = std::get_if<double>(&x.v)) return *p;
  throw std::runtime_error("expected real");
}

const std::string& as_text(const Value& x) {
  if (auto p = std::get_if<std::string>(&x.v)) return *p;
  throw std::runtime_error("expected text");
}

const Record& as_record(const Value& x) {
  if (auto p = std::get_if<Record>(&x.v)) return *p;
  throw std::runtime_error("expected record");
}

Record& as_record(Value& x) {
  if (auto p = std::get_if<Record>(&x.v)) return *p;
  throw std::runtime_error("expected record");
}

const Array& as_array(const Value& x) {
  if (auto p = std::get_if<Array>(&x.v)) return *p;
  throw std::runtime_error("expected array");
}

Array& as_array(Value& x) {
  if (auto p = std::get_if<Array>(&x.v)) return *p;
  throw std::runtime_error("expected array");
}

const Tuple& as_tuple(const Value& x) {
  if (auto p = std::get_if<Tuple>(&x.v)) return *p;
  throw std::runtime_error("expected tuple");
}

Tuple& as_tuple(Value& x) {
  if (auto p = std::get_if<Tuple>(&x.v)) return *p;
  throw std::runtime_error("expected tuple");
}

} // namespace caps
