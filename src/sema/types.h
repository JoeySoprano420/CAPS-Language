#pragma once
#include <string>
#include <vector>
#include <optional>

#include "ast/ast.h"

struct Type {
  enum class Kind { Int, Bool, Real, Text, Channel, Result, Unknown } kind = Kind::Unknown;

  // Channel<T;N>
  std::optional<int> chan_cap;
  std::unique_ptr<Type> elem;

  // Result<T,E>
  std::unique_ptr<Type> ok;
  std::unique_ptr<Type> err;

  static Type Int() { Type t; t.kind=Kind::Int; return t; }
  static Type Bool() { Type t; t.kind=Kind::Bool; return t; }
  static Type Real() { Type t; t.kind=Kind::Real; return t; }
  static Type Text() { Type t; t.kind=Kind::Text; return t; }
  static Type Unknown() { Type t; t.kind=Kind::Unknown; return t; }

  static Type Channel(Type elem, int cap) {
    Type t; t.kind=Kind::Channel;
    t.elem = std::make_unique<Type>(std::move(elem));
    t.chan_cap = cap;
    return t;
  }

  static Type Result(Type ok, Type err) {
    Type t; t.kind=Kind::Result;
    t.ok = std::make_unique<Type>(std::move(ok));
    t.err = std::make_unique<Type>(std::move(err));
    return t;
  }
};

bool type_equals(const Type& a, const Type& b);
std::string type_to_string(const Type& t);

Type type_from_typeref(const TypeRef& r);
TypeRef typeref_from_type(const Type& t);
