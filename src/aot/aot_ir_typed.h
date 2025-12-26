#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <stdexcept>

namespace caps::aot {

// ===== Types =====

enum class TypeKind {
  I64,
  Bool,
  F64,
  Text,
  // Result<T,E> where E is text for now (matches your plan)
  ResultBoolText,   // Result<bool,text>
  ResultI64Text,    // Result<i64,text>
  ResultTextText,   // Result<text,text>
};

struct Type {
  TypeKind kind{};
  std::string debug;

  static Type i64() { return {TypeKind::I64, "i64"}; }
  static Type boolean() { return {TypeKind::Bool, "bool"}; }
  static Type f64() { return {TypeKind::F64, "f64"}; }
  static Type text() { return {TypeKind::Text, "text"}; }
  static Type result_bool_text() { return {TypeKind::ResultBoolText, "Result<bool,text>"}; }
  static Type result_i64_text() { return {TypeKind::ResultI64Text, "Result<i64,text>"}; }
  static Type result_text_text() { return {TypeKind::ResultTextText, "Result<text,text>"}; }
};

inline bool is_result(TypeKind k) {
  return k == TypeKind::ResultBoolText || k == TypeKind::ResultI64Text || k == TypeKind::ResultTextText;
}

// ===== Typed expressions =====

struct Expr {
  enum class Kind {
    LitI64, LitBool, LitF64, LitText,
    Var,
    BinOp,      // typed binop
    // Result accessors (since you desugar ? and use ok/value/error in sema)
    ResultOk,     // ok(exprResult)
    ResultValue,  // value(exprResult)
    ResultError,  // error(exprResult) -> text
  } kind{};

  Type type;

  std::string var;     // Var
  std::string op;      // BinOp
  int64_t i64{};
  bool b{};
  double f64{};
  std::string text;

  std::vector<Expr> args; // BinOp 2 args; Result* 1 arg

  static Expr v(std::string n, Type t) { Expr e; e.kind=Kind::Var; e.var=std::move(n); e.type=t; return e; }
};

// ===== Actions / transitions =====

struct Action {
  enum class Kind {
    Assign,      // dst = expr
    Send,        // send chan <- expr
    Receive,     // recv chan -> dst
    TrySend,     // dst(Result<bool,text>) = try_send chan <- expr
    TryReceive,  // dst(Result<T,text>) = try_receive chan
  } kind{};

  std::string dst;
  std::string chan;
  Expr expr;          // Assign/Send/TrySend
  Type recv_type;     // Receive/TryReceive element type
};

struct Transition {
  enum class Kind { Goto, IfElse } kind{};

  // IfElse
  Expr cond; // bool
  std::string then_state;
  std::string else_state;
  std::vector<Action> then_actions; // carry then/else action lists
  std::vector<Action> else_actions;

  // Goto
  std::string to_state;
};

struct State {
  std::string name;
  bool terminal = false;
  std::vector<Action> actions;
  Transition tr;
};

struct ChannelDecl {
  std::string name;
  Type elem_type;
  uint32_t capacity = 0; // 0 = rendezvous (optional; see note)
};

struct Process {
  std::string name;
  std::string initial_state;

  // locals include __last_error:text and any temps you used for ? lowering
  std::vector<std::pair<std::string, Type>> locals;

  // states must include "__Error" if the process uses ? lowering
  std::unordered_map<std::string, State> states;
};

struct Group {
  std::string name;
  std::vector<std::string> annotations; // pipeline_safe/realtimesafe etc (AOT may ignore)
  std::vector<ChannelDecl> channels;
  std::vector<Process> processes;
  std::vector<std::string> schedule; // step names = process names
  bool repeat = true;
};

} // namespace caps::aot
