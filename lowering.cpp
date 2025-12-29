#include "ir/lowering.h"
#include "sema/types.h"

IRGroup Lowering::lower_group(const GroupDecl& g) {
  IRGroup out;
  out.name = g.name;
  out.schedule_steps = g.schedule.steps;

  for (auto& p : g.processes) out.processes.push_back(lower_process(p));
  return out;
}

IRProcess Lowering::lower_process(const ProcessDecl& p) {
  IRProcess out;
  out.name = p.name;
  for (auto& ob : p.on_blocks) out.states.push_back(lower_on_block(ob));
  return out;
}

IRState Lowering::lower_on_block(const OnBlock& ob) {
  IRState st;
  st.name = ob.state_name;

  for (auto& a : ob.actions) lower_action(a, st.actions);

  IRTransition tr;
  tr.pos = ob.transition.pos;
  if (ob.transition.kind == Transition::Kind::Unconditional) {
    tr.kind = IRTransition::Kind::Unconditional;
    tr.to = ob.transition.to_state;
  } else {
    tr.kind = IRTransition::Kind::IfElse;
    tr.cond = lower_expr(ob.transition.cond);
    tr.then_to = ob.transition.then_state;
    tr.else_to = ob.transition.else_state;
    for (auto& a : ob.transition.then_actions) lower_action(a, tr.then_actions);
    for (auto& a : ob.transition.else_actions) lower_action(a, tr.else_actions);
  }

  st.transition = std::move(tr);
  return st;
}

void Lowering::lower_stmt_as_actions(const Stmt& st, std::vector<IRAction>& out) {
  // Desugar: do x = rr?  => TryUnwrapAssign(dst=x, operand=rr, errorState=__Error, lastError=__last_error)
  if ((st.kind == Stmt::Kind::Assign || st.kind == Stmt::Kind::Let || st.kind == Stmt::Kind::Var) &&
      st.expr.kind == Expr::Kind::Try) {
    IRAction a;
    a.kind = IRAction::Kind::TryUnwrapAssign;
    a.pos = st.pos;
    a.unwrap_dst = st.name;
    a.unwrap_result_expr = lower_expr(st.expr.args[0]);
    a.unwrap_error_state = "__Error";
    a.unwrap_last_error = "__last_error";
    out.push_back(std::move(a));
    return;
  }

  if (st.kind == Stmt::Kind::Assign || st.kind == Stmt::Kind::Let || st.kind == Stmt::Kind::Var) {
    IRAction a;
    a.kind = IRAction::Kind::Assign;
    a.pos = st.pos;
    a.dst = st.name;
    a.expr = lower_expr(st.expr);
    out.push_back(std::move(a));
    return;
  }
}

void Lowering::lower_action(const Action& a, std::vector<IRAction>& out) {
  if (a.kind == Action::Kind::DoStmt) {
    lower_stmt_as_actions(a.stmt, out);
    return;
  }

  if (a.kind == Action::Kind::Send) {
    IRAction ia;
    ia.kind = IRAction::Kind::Send;
    ia.pos = a.pos;
    ia.chan = a.chan;
    ia.expr = lower_expr(a.send_expr);
    out.push_back(std::move(ia));
    return;
  }

  if (a.kind == Action::Kind::Receive) {
    IRAction ia;
    ia.kind = IRAction::Kind::Receive;
    ia.pos = a.pos;
    ia.chan = a.chan;
    ia.dst = a.recv_target;
    ia.recv_declares = a.recv_declares;
    out.push_back(std::move(ia));
    return;
  }

  if (a.kind == Action::Kind::TrySend) {
    IRAction ia;
    ia.kind = IRAction::Kind::TrySend;
    ia.pos = a.pos;
    ia.chan = a.try_send_chan;
    ia.try_send_expr = lower_expr(a.try_send_expr);
    ia.try_send_out = a.try_send_outvar;
    out.push_back(std::move(ia));
    return;
  }

  if (a.kind == Action::Kind::TryReceive) {
    IRAction ia;
    ia.kind = IRAction::Kind::TryReceive;
    ia.pos = a.pos;
    ia.chan = a.try_recv_chan;
    ia.try_recv_out = a.try_recv_outvar;
    out.push_back(std::move(ia));
    return;
  }
}

IRExpr Lowering::lower_expr(const Expr& e) {
  IRExpr ie;
  ie.type = type_from_typeref(e.inferred_type);
  switch (e.kind) {
    case Expr::Kind::Ident: {
      ie.kind = IRExpr::Kind::Var;
      ie.var = e.text;
      return ie;
    }
    case Expr::Kind::IntLit: {
      ie.kind = IRExpr::Kind::LitI64;
      ie.i64 = std::stoll(e.text);
      return ie;
    }
    case Expr::Kind::RealLit: {
      ie.kind = IRExpr::Kind::LitF64;
      ie.f64 = std::stod(e.text);
      return ie;
    }
    case Expr::Kind::TextLit: {
      ie.kind = IRExpr::Kind::LitText;
      ie.text = e.text;
      return ie;
    }
    case Expr::Kind::Binary: {
      ie.kind = IRExpr::Kind::BinOp;
      ie.op = e.text;
      ie.args = {lower_expr(e.args[0]), lower_expr(e.args[1])};
      return ie;
    }
    case Expr::Kind::Call: {
      ie.kind = IRExpr::Kind::Call;
      ie.func_name = e.args[0].text; // assume callee is ident
      for (size_t i = 1; i < e.args.size(); ++i) {
        ie.args.push_back(lower_expr(e.args[i]));
      }
      return ie;
    }
    case Expr::Kind::Try: {
      // Should not reach here if desugared
      throw std::runtime_error("Try not desugared");
    }
    default: {
      throw std::runtime_error("Unknown Expr kind");
    }
  }
}

// IR lowering for new features
IRValue Lowering::lower_match_expr(const MatchExpr& expr) {
  // Implement match expression lowering here
}

IRValue Lowering::lower_array_expr(const ArrayExpr& expr) {
  // Implement array expression lowering here
}

void Lowering::lower_for_loop(const ForLoop& loop) {
  // Implement for loop lowering here
}

IRFunction Lowering::lower_const_fn(const ConstFnDecl& fn) {
  // Interpret body at compile-time
  // Return constant result
}

// IR lowering enhancements for dead code elimination, channel fusion, process inlining,
// static memory layout, zero-copy message passing

void Lowering::lower_with_optimizations(const Program& prog, IRProgram& ir) {
  // Standard lowering
  lower_program(prog, ir);

  // Apply optimizations
  dead_code_elimination(ir);
  channel_fusion(ir);
  process_inlining(ir);
  static_memory_layout(ir);
  zero_copy_message_passing(ir);
}

// Dead code elimination: remove unused functions, variables, states
void Lowering::dead_code_elimination(IRProgram& ir) {
  // Analyze usage and remove unreachable code
}

// Channel fusion: merge channels with same type and usage
void Lowering::channel_fusion(IRProgram& ir) {
  // Fuse compatible channels
}

// Process inlining: inline small processes
void Lowering::process_inlining(IRProgram& ir) {
  // Inline processes below size threshold
}

// Static memory layout: pre-allocate all memory
void Lowering::static_memory_layout(IRProgram& ir) {
  // Assign static addresses
}

// Zero-copy message passing: use references instead of copies
void Lowering::zero_copy_message_passing(IRProgram& ir) {
  // Modify sends to pass pointers
}
