#pragma once
#include <string>
#include <vector>
#include "ast/ast.h"

using IRExpr = Expr;

struct IRAction {
  enum class Kind {
    Assign,
    Send,
    Receive,
    TrySend,
    TryReceive,
    TryUnwrapAssign,
  } kind;

  SourcePos pos;

  // Assign
  std::string dst;
  IRExpr expr;

  // Send/Receive
  std::string chan;
  bool recv_declares = false;

  // TrySend
  std::string try_send_out;
  IRExpr try_send_expr;

  // TryReceive
  std::string try_recv_out;

  // TryUnwrapAssign
  std::string unwrap_dst;
  IRExpr unwrap_result_expr;
  std::string unwrap_error_state;  // "__Error"
  std::string unwrap_last_error;   // "__last_error"
};

struct IRTransition {
  enum class Kind { Unconditional, IfElse } kind;
  SourcePos pos;

  std::string to;

  IRExpr cond;
  std::string then_to;
  std::string else_to;

  std::vector<IRAction> then_actions;
  std::vector<IRAction> else_actions;
};

struct IRState {
  std::string name;
  std::vector<IRAction> actions;
  IRTransition transition;
};

struct IRProcess {
  std::string name;
  std::vector<IRState> states;
};

struct IRGroup {
  std::string name;
  std::vector<IRProcess> processes;
  std::vector<std::string> schedule_steps;
};

struct IRExpr {
  enum class Kind {
    LitI64, LitBool, LitF64, LitText,
    Var,
    BinOp,      // typed binop
    // Result accessors (since you desugar ? and use ok/value/error in sema)
    ResultOk,     // ok(exprResult)
    ResultValue,  // value(exprResult)
    ResultError,  // error(exprResult) -> text
    Call,         // function call
  } kind{};

  Type type;

  std::string var;     // Var
  std::string op;      // BinOp
  int64_t i64{};
  bool b{};
  double f64{};
  std::string text;

  std::string func_name; // Call
  std::vector<IRExpr> args; // BinOp 2 args; Result* 1 arg; Call args

  static IRExpr v(std::string n, Type t) { IRExpr e; e.kind=IRExpr::Kind::Var; e.var=std::move(n); e.type=t; return e; }
};
