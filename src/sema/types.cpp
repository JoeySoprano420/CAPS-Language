#include "sema/types.h"

bool type_equals(const Type& a, const Type& b) {
  if (a.kind != b.kind) return false;
  switch (a.kind) {
    case Type::Kind::Channel:
      if (!a.elem || !b.elem) return false;
      return *a.chan_cap == *b.chan_cap && type_equals(*a.elem, *b.elem);
    case Type::Kind::Result:
      return type_equals(*a.ok, *b.ok) && type_equals(*a.err, *b.err);
    default:
      return true;
  }
}

std::string type_to_string(const Type& t) {
  switch (t.kind) {
    case Type::Kind::Int: return "int";
    case Type::Kind::Bool: return "bool";
    case Type::Kind::Real: return "real";
    case Type::Kind::Text: return "text";
    case Type::Kind::Channel:
      return "channel<" + type_to_string(*t.elem) + ";" + std::to_string(*t.chan_cap) + ">";
    case Type::Kind::Result:
      return "Result<" + type_to_string(*t.ok) + "," + type_to_string(*t.err) + ">";
    default: return "unknown";
  }
}

Type type_from_typeref(const TypeRef& r) {
  if (r.is_channel) {
    Type elem = type_from_typeref(r.args.at(0));
    return Type::Channel(std::move(elem), r.channel_capacity);
  }

  if (r.name == "int") return Type::Int();
  if (r.name == "bool") return Type::Bool();
  if (r.name == "real") return Type::Real();
  if (r.name == "text") return Type::Text();

  if (r.name == "Result" && r.args.size() == 2) {
    Type ok = type_from_typeref(r.args[0]);
    Type err = type_from_typeref(r.args[1]);
    return Type::Result(std::move(ok), std::move(err));
  }

  return Type::Unknown();
}

TypeRef typeref_from_type(const Type& t) {
  TypeRef r;
  switch (t.kind) {
    case Type::Kind::Int: r.name="int"; break;
    case Type::Kind::Bool: r.name="bool"; break;
    case Type::Kind::Real: r.name="real"; break;
    case Type::Kind::Text: r.name="text"; break;
    case Type::Kind::Channel:
      r.is_channel = true;
      r.name = "channel";
      r.args = { typeref_from_type(*t.elem) };
      r.channel_capacity = *t.chan_cap;
      break;
    case Type::Kind::Result:
      r.name = "Result";
      r.args = { typeref_from_type(*t.ok), typeref_from_type(*t.err) };
      break;
    default:
      r.name = "unknown";
      break;
  }
  return r;
}
