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
