#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

namespace caps {

enum class IRTypeKind { Int, Bool, Real, Text, Record, Array, Tuple, ResultAny };

struct IRType {
  IRTypeKind kind = IRTypeKind::Int;
  std::string name; // for debug
};

struct IRExpr {
  enum class Kind {
    LitInt, LitBool, LitReal, LitText,
    Var,
    BinOp,      // + - * / == != < > <= >= && ||
    Field,      // expr.field
    Index,      // expr[index]
    LenChannel, // len(channelName)
  } kind;

  std::string op;         // for BinOp
  std::string name;       // for Var / LenChannel
  std::string field;      // for Field
  int64_t lit_i = 0;
  bool lit_b = false;
  double lit_r = 0.0;
  std::string lit_s;

  int64_t index_const = 0;

  std::vector<IRExpr> args; // BinOp has 2 args; Field has 1; Index has 1; etc.

  static IRExpr var(std::string n) { IRExpr e; e.kind=Kind::Var; e.name=std::move(n); return e; }
};

struct IRAction {
  enum class Kind {
    Assign,
    Send,
    Receive,
    TrySend,
    TryReceive,
    // Optional: Assert, etc.
    // Note: ? is not an action; it is desugared into transitions into __Error + __last_error handling.
  } kind;

  std::string dst; // Assign/Receive/Try* result var
  IRExpr expr;     // Assign or Send expr or TrySend expr
  std::string chan; // Send/Receive channel name
};

struct IRTransition {
  enum class Kind { Goto, IfElse } kind;

  // IfElse
  IRExpr cond;

  std::string then_state;
  std::string else_state;

  // carry then/else action lists (as requested)
  std::vector<IRAction> then_actions;
  std::vector<IRAction> else_actions;

  // Goto
  std::string to_state;
};

struct IRState {
  std::string name;
  bool terminal = false;
  std::vector<IRAction> actions;
  IRTransition transition;
};

struct IRProcess {
  std::string name;
  std::string initial_state;
  std::unordered_map<std::string, IRState> states;

  // locals schema for debug + initialization
  std::vector<std::string> local_names;

  // outputs (written by process, read after finish)
  std::vector<std::string> output_names;

  // special error state name for ? desugaring
  // convention: "__Error" exists if process uses ? and has error path
};

struct IRChannelDecl {
  std::string name;
  size_t capacity = 0;
  IRType elem_type;
};

struct IRSchedule {
  std::vector<std::string> steps; // process names
  bool repeat = true;
};

struct IRGroup {
  std::string name;
  std::vector<std::string> annotations; // e.g. "pipeline_safe", "realtimesafe"
  std::vector<IRChannelDecl> channels;
  std::vector<IRProcess> processes;
  IRSchedule schedule;
};

struct IRProgram {
  std::vector<IRGroup> groups;
};

} // namespace caps
