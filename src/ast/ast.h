#pragma once
#include <string>
#include <vector>
#include <optional>
#include <variant>
#include <unordered_map>
#include "util/diag.h"

struct TypeRef {
  std::string name; // "int", "text", "Result", user types
  std::vector<TypeRef> args; // generics
  bool is_channel = false;
  int channel_capacity = -1; // for channel<T;N>

  static TypeRef named(const std::string& n) { return TypeRef{n}; }
};

struct Expr {
  enum class Kind {
    Ident,
    IntLit,
    RealLit,
    TextLit,
    Binary,
    Call,
    Try, // postfix ?
  } kind = Kind::Ident;

  SourcePos pos;
  std::string text; // ident or literal or operator for Binary
  std::vector<Expr> args; // Binary: [lhs,rhs], Call: [callee,args...], Try: [operand]

  // filled by sema
  TypeRef inferred_type;

  static Expr ident(SourcePos p, const std::string& n) { Expr e; e.kind=Kind::Ident; e.pos=p; e.text=n; return e; }
  static Expr text_literal(SourcePos p, const std::string& s) { Expr e; e.kind=Kind::TextLit; e.pos=p; e.text=s; return e; }
};

struct Stmt {
  enum class Kind { Let, Var, Assign, Return, ExprStmt } kind = Kind::ExprStmt;
  SourcePos pos;
  std::string name;     // let/var/assign target
  std::optional<TypeRef> explicit_type;
  Expr expr;            // initializer or rhs
};

struct Action {
  enum class Kind { DoStmt, Send, Receive, TrySend, TryReceive } kind = Kind::DoStmt;
  SourcePos pos;

  // DoStmt
  Stmt stmt;

  // Send
  Expr send_expr;
  std::string chan;

  // Receive
  std::string recv_target;
  bool recv_declares = false;
  std::optional<TypeRef> recv_type;

  // TrySend
  Expr try_send_expr;
  std::string try_send_chan;
  std::string try_send_outvar;

  // TryReceive
  std::string try_recv_chan;
  std::string try_recv_outvar;
};

struct Transition {
  enum class Kind { Unconditional, IfElse } kind = Kind::Unconditional;
  SourcePos pos;

  // unconditional
  std::string to_state;

  // IfElse
  Expr cond;
  std::vector<Action> then_actions;
  std::vector<Action> else_actions;
  std::string then_state;
  std::string else_state;
};

struct OnBlock {
  SourcePos pos;
  std::string state_name;
  std::vector<Action> actions;
  Transition transition;
};

struct Param {
  std::string name;
  TypeRef type;
  SourcePos pos;
};

struct Annotation {
  std::string name;           // pipeline_safe, realtimesafe, max_sends, max_receives
  SourcePos pos;
  std::vector<std::string> args; // raw tokens inside (...)
};

struct ChannelDecl {
  SourcePos pos;
  std::string name;
  TypeRef elem_type;
  int capacity = 0; // channel<T> => 0, channel<T;N> => N
};

struct ProcessDecl {
  SourcePos pos;
  std::string name;
  std::vector<Annotation> annotations;
  std::vector<Param> inputs;
  std::vector<Param> outputs;

  std::vector<std::string> states;
  std::vector<Stmt> locals;
  std::vector<OnBlock> on_blocks;
};

struct ScheduleDecl {
  SourcePos pos;
  std::vector<std::string> steps; // process names
  bool repeat = false;
};

struct GroupDecl {
  SourcePos pos;
  std::string name;
  std::vector<Annotation> annotations;
  std::vector<ChannelDecl> channels;
  std::vector<ProcessDecl> processes;
  ScheduleDecl schedule;
};

struct ModuleDecl {
  SourcePos pos;
  std::string name;
};

struct Program {
  ModuleDecl module;
  std::vector<GroupDecl> groups;
};
