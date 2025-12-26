#include "ir/lowering.h"

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
    tr.cond = ob.transition.cond;
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
    a.unwrap_result_expr = st.expr.args[0];
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
    a.expr = st.expr;
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
    ia.expr = a.send_expr;
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
    ia.try_send_expr = a.try_send_expr;
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
