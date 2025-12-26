#include "backend/eval.h"
#include <stdexcept>

namespace caps {

static Value eval_binop(const std::string& op, const Value& a, const Value& b) {
  if (op == "+") return Value::i(as_int(a) + as_int(b));
  if (op == "-") return Value::i(as_int(a) - as_int(b));
  if (op == "*") return Value::i(as_int(a) * as_int(b));
  if (op == "/") return Value::i(as_int(a) / as_int(b));

  if (op == "==") {
    // minimal: int/bool/text compare
    if (a.v.index() != b.v.index()) return Value::b(false);
    if (std::holds_alternative<int64_t>(a.v)) return Value::b(as_int(a) == as_int(b));
    if (std::holds_alternative<bool>(a.v)) return Value::b(as_bool(a) == as_bool(b));
    if (std::holds_alternative<std::string>(a.v)) return Value::b(as_text(a) == as_text(b));
    throw std::runtime_error("== unsupported type");
  }
  if (op == "!=") {
    auto eq = eval_binop("==", a, b);
    return Value::b(!as_bool(eq));
  }
  if (op == "<") return Value::b(as_int(a) < as_int(b));
  if (op == "<=") return Value::b(as_int(a) <= as_int(b));
  if (op == ">") return Value::b(as_int(a) > as_int(b));
  if (op == ">=") return Value::b(as_int(a) >= as_int(b));

  if (op == "&&") return Value::b(as_bool(a) && as_bool(b));
  if (op == "||") return Value::b(as_bool(a) || as_bool(b));

  throw std::runtime_error("unknown binop: " + op);
}

Value eval_expr(const Runtime& rt, const ProcessInstance* self, const IRExpr& e) {
  using K = IRExpr::Kind;
  switch (e.kind) {
    case K::LitInt: return Value::i(e.lit_i);
    case K::LitBool: return Value::b(e.lit_b);
    case K::LitReal: return Value::r(e.lit_r);
    case K::LitText: return Value::s(e.lit_s);

    case K::Var: {
      if (!self) throw std::runtime_error("Var requires process context: " + e.name);
      auto it = self->locals.find(e.name);
      if (it != self->locals.end()) return it->second;
      auto ot = self->outputs.find(e.name);
      if (ot != self->outputs.end()) return ot->second;
      throw std::runtime_error("unknown var: " + e.name);
    }

    case K::LenChannel: {
      auto it = rt.channels.find(e.name);
      if (it == rt.channels.end()) throw std::runtime_error("unknown channel: " + e.name);
      return Value::i((int64_t)it->second.buffer.size());
    }

    case K::BinOp: {
      if (e.args.size() != 2) throw std::runtime_error("BinOp expects 2 args");
      Value a = eval_expr(rt, self, e.args[0]);
      Value b = eval_expr(rt, self, e.args[1]);
      return eval_binop(e.op, a, b);
    }

    case K::Field: {
      if (e.args.size() != 1) throw std::runtime_error("Field expects 1 arg");
      Value base = eval_expr(rt, self, e.args[0]);
      auto& rec = as_record(base);
      auto it = rec.fields.find(e.field);
      if (it == rec.fields.end()) throw std::runtime_error("missing field: " + e.field);
      return it->second;
    }

    case K::Index: {
      if (e.args.size() != 1) throw std::runtime_error("Index expects 1 arg");
      Value base = eval_expr(rt, self, e.args[0]);
      auto& arr = as_array(base);
      if (e.index_const < 0 || (size_t)e.index_const >= arr.elems.size())
        throw std::runtime_error("index out of bounds");
      return arr.elems[(size_t)e.index_const];
    }
  }
  throw std::runtime_error("unhandled expr kind");
}

} // namespace caps
