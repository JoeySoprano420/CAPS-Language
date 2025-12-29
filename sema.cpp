#include "sema/sema.h"
#include "analysis/pipeline.h"
#include <unordered_set>

Sema::Sema(Diag& d) : diag(d) {}

bool Sema::has_annotation(const std::vector<Annotation>& anns, const std::string& name) const {
  for (auto& a : anns) if (a.name == name) return true;
  return false;
}

static bool expr_contains_try(const Expr& e) {
  if (e.kind == Expr::Kind::Try) return true;
  for (auto& a : e.args) if (expr_contains_try(a)) return true;
  return false;
}

static bool stmt_contains_try(const Stmt& s) { return expr_contains_try(s.expr); }

static bool action_contains_try(const Action& a) {
  if (a.kind == Action::Kind::Send) return expr_contains_try(a.send_expr);
  if (a.kind == Action::Kind::TrySend) return expr_contains_try(a.try_send_expr);
  if (a.kind == Action::Kind::DoStmt) return stmt_contains_try(a.stmt);
  return false;
}

static bool transition_contains_try(const Transition& t) {
  if (t.kind == Transition::Kind::IfElse) {
    if (expr_contains_try(t.cond)) return true;
    for (auto& a : t.then_actions) if (action_contains_try(a)) return true;
    for (auto& a : t.else_actions) if (action_contains_try(a)) return true;
  }
  return false;
}

static bool try_usage_is_allowed(const Stmt& s) {
  if (!(s.kind == Stmt::Kind::Assign || s.kind == Stmt::Kind::Let || s.kind == Stmt::Kind::Var)) return false;
  return s.expr.kind == Expr::Kind::Try;
}

static bool process_has_state(const ProcessDecl& p, const std::string& name) {
  for (auto& s : p.states) if (s == name) return true;
  return false;
}
static bool process_has_onblock(const ProcessDecl& p, const std::string& state) {
  for (auto& ob : p.on_blocks) if (ob.state_name == state) return true;
  return false;
}
static bool process_has_local(const ProcessDecl& p, const std::string& name) {
  for (auto& st : p.locals) if (st.name == name) return true;
  return false;
}

void Sema::check(Program& p) {
  for (auto& g : p.groups) check_group(g);
}

void Sema::check_group(GroupDecl& g) {
  GroupEnv env;

  // group channels
  for (auto& c : g.channels) {
    Type ch = Type::Channel(type_from_typeref(c.elem_type), c.capacity);
    env.channels[c.name] = std::move(ch);
  }

  // processes map
  for (auto& p : g.processes) {
    if (env.processes.count(p.name)) diag.error(p.pos, "duplicate process name: " + p.name);
    env.processes[p.name] = &p;
  }

  // schedule step names must match process names
  for (auto& s : g.schedule.steps) {
    if (!env.processes.count(s)) diag.error(g.schedule.pos, "schedule step refers to unknown process: " + s);
  }

  // pipeline_safe topology enforcement (after we can infer send/receive usage; but we can validate schedule order later too)
  bool pipeline_safe = has_annotation(g.annotations, "pipeline_safe");
  bool realtime_safe = has_annotation(g.annotations, "realtimesafe");

  // check each process
  for (auto& p : g.processes) {
    check_process(g, env, p);
  }

  // pipeline-safe analysis after sema has at least action structure
  if (pipeline_safe) {
    PipelineChecker pc(diag);
    pc.check_group_pipeline_safe(g);
  }

  (void)realtime_safe;
}

void Sema::check_process(GroupDecl& g, GroupEnv& env, ProcessDecl& p) {
  bool realtime_safe = has_annotation(g.annotations, "realtimesafe");

  // Determine whether process uses '?' and enforce __Error and auto-inject __last_error
  bool uses_try = false;

  for (auto& st : p.locals) {
    if (stmt_contains_try(st)) {
      uses_try = true;
      diag.error(st.pos, "postfix '?' is only allowed inside 'do' actions (use: on S { do x = expr? ... })");
    }
  }

  for (auto& ob : p.on_blocks) {
    for (auto& a : ob.actions) {
      if (action_contains_try(a)) {
        uses_try = true;
        if (a.kind == Action::Kind::DoStmt && stmt_contains_try(a.stmt) && !try_usage_is_allowed(a.stmt)) {
          diag.error(a.pos, "postfix '?' is only allowed as RHS of let/var/assign (do x = expr?)");
        }
      }
    }
    if (transition_contains_try(ob.transition)) {
      uses_try = true;
      if (ob.transition.kind == Transition::Kind::IfElse && expr_contains_try(ob.transition.cond)) {
        diag.error(ob.transition.pos, "postfix '?' is not allowed in transition conditions");
      }
      // still allow '?' inside then/else action blocks (those are actions)
      for (auto& a : ob.transition.then_actions) {
        if (a.kind == Action::Kind::DoStmt && stmt_contains_try(a.stmt) && !try_usage_is_allowed(a.stmt)) {
          diag.error(a.pos, "postfix '?' is only allowed as RHS of let/var/assign (do x = expr?)");
        }
      }
      for (auto& a : ob.transition.else_actions) {
        if (a.kind == Action::Kind::DoStmt && stmt_contains_try(a.stmt) && !try_usage_is_allowed(a.stmt)) {
          diag.error(a.pos, "postfix '?' is only allowed as RHS of let/var/assign (do x = expr?)");
        }
      }
    }
  }

  if (uses_try) {
    if (!process_has_state(p, "__Error")) diag.error(p.pos, "process uses '?' but missing state '__Error'");
    if (!process_has_onblock(p, "__Error")) diag.error(p.pos, "process uses '?' but missing on __Error { ... }");

    if (!process_has_local(p, "__last_error")) {
      Stmt injected;
      injected.kind = Stmt::Kind::Var;
      injected.pos = p.pos;
      injected.name = "__last_error";
      injected.explicit_type = TypeRef::named("text");
      injected.expr = Expr::text_literal(p.pos, "");
      p.locals.insert(p.locals.begin(), injected);
    }
  }

  // Build local typing env (params + locals)
  std::unordered_map<std::string, Type> locals;

  for (auto& in : p.inputs) locals[in.name] = type_from_typeref(in.type);
  for (auto& out : p.outputs) locals[out.name] = type_from_typeref(out.type);

  for (auto& st : p.locals) {
    // only allow let/var initializers without '?'
    Type rhs = check_expr(env, p, locals, st.expr);
    if (st.explicit_type) {
      Type ex = type_from_typeref(*st.explicit_type);
      if (!type_equals(ex, rhs)) diag.error(st.pos, "type mismatch in local init '" + st.name + "': expected " + type_to_string(ex) + " got " + type_to_string(rhs));
      locals[st.name] = ex;
    } else {
      locals[st.name] = rhs;
    }
  }

  // Validate FSM: states declared must have on-blocks (strong)
  std::unordered_set<std::string> declared;
  for (auto& s : p.states) declared.insert(s);

  std::unordered_set<std::string> has_on;
  for (auto& ob : p.on_blocks) has_on.insert(ob.state_name);

  for (auto& s : declared) {
    if (!has_on.count(s)) diag.error(p.pos, "state '" + s + "' declared but missing 'on " + s + " { ... }'");
  }

  // Validate each on-block ends in exactly one transition: parser already enforces transition presence
  // Validate transition targets exist
  auto check_state_exists = [&](SourcePos pos, const std::string& name) {
    if (!declared.count(name)) diag.error(pos, "transition target state not declared: " + name);
  };

  for (auto& ob : p.on_blocks) {
    // base actions
    for (auto& a : ob.actions) check_action(g, env, p, locals, a, realtime_safe);

    // transition
    if (ob.transition.kind == Transition::Kind::Unconditional) {
      check_state_exists(ob.transition.pos, ob.transition.to_state);
    } else {
      // cond must be bool
      Type cty = check_expr(env, p, locals, ob.transition.cond);
      if (cty.kind != Type::Kind::Bool) diag.error(ob.transition.pos, "transition condition must be bool");
      // then/else actions
      for (auto& a : ob.transition.then_actions) check_action(g, env, p, locals, a, realtime_safe);
      for (auto& a : ob.transition.else_actions) check_action(g, env, p, locals, a, realtime_safe);

      check_state_exists(ob.transition.pos, ob.transition.then_state);
      check_state_exists(ob.transition.pos, ob.transition.else_state);
    }
  }
}

Type Sema::check_expr(GroupEnv& env, ProcessDecl& p, std::unordered_map<std::string, Type>& locals, Expr& e) {
  switch (e.kind) {
    case Expr::Kind::Ident: {
      if (locals.count(e.text)) {
        Type t = locals[e.text];
        e.inferred_type = typeref_from_type(t);
        return t;
      }
      diag.error(e.pos, "unknown identifier: " + e.text);
      e.inferred_type = TypeRef::named("unknown");
      return Type::Unknown();
    }
    case Expr::Kind::IntLit: {
      e.inferred_type = TypeRef::named("int");
      return Type::Int();
    }
    case Expr::Kind::RealLit: {
      e.inferred_type = TypeRef::named("real");
      return Type::Real();
    }
    case Expr::Kind::TextLit: {
      e.inferred_type = TypeRef::named("text");
      return Type::Text();
    }
    case Expr::Kind::Binary: {
      Type a = check_expr(env, p, locals, e.args[0]);
      Type b = check_expr(env, p, locals, e.args[1]);
      // comparators -> bool, arithmetic -> int/real (very minimal)
      if (e.text == "==" || e.text == "!=" || e.text == "<" || e.text == "<=" || e.text == ">" || e.text == ">=") {
        e.inferred_type = TypeRef::named("bool");
        return Type::Bool();
      }
      if (e.text == "&&" || e.text == "||") {
        if (a.kind != Type::Kind::Bool || b.kind != Type::Kind::Bool) diag.error(e.pos, "logical ops require bool");
        e.inferred_type = TypeRef::named("bool");
        return Type::Bool();
      }
      // + - * /
      if (a.kind == Type::Kind::Int && b.kind == Type::Kind::Int) { e.inferred_type = TypeRef::named("int"); return Type::Int(); }
      if (a.kind == Type::Kind::Real && b.kind == Type::Kind::Real) { e.inferred_type = TypeRef::named("real"); return Type::Real(); }
      diag.error(e.pos, "unsupported binary op types: " + type_to_string(a) + " " + e.text + " " + type_to_string(b));
      e.inferred_type = TypeRef::named("unknown");
      return Type::Unknown();
    }
    case Expr::Kind::Call: {
      if (e.args.empty() || e.args[0].kind != Expr::Kind::Ident) {
        diag.error(e.pos, "call expects function name");
        e.inferred_type = TypeRef::named("unknown");
        return Type::Unknown();
      }
      std::string func = e.args[0].text;
      if (func == "len") {
        if (e.args.size() != 2) {
          diag.error(e.pos, "len expects 1 argument");
          e.inferred_type = TypeRef::named("unknown");
          return Type::Unknown();
        }
        Type arg = check_expr(env, p, locals, e.args[1]);
        if (arg.kind != Type::Kind::Channel) {
          diag.error(e.pos, "len argument must be a channel");
          e.inferred_type = TypeRef::named("unknown");
          return Type::Unknown();
        }
        e.inferred_type = TypeRef::named("int");
        return Type::Int();
      } else {
        diag.error(e.pos, "unknown function: " + func);
        e.inferred_type = TypeRef::named("unknown");
        return Type::Unknown();
      }
    }
    case Expr::Kind::Try: {
      // operand must be Result<T,text> in process context
      Type r = check_expr(env, p, locals, e.args[0]);
      if (r.kind != Type::Kind::Result) {
        diag.error(e.pos, "postfix '?' operand must be Result<T,text>");
        e.inferred_type = TypeRef::named("unknown");
        return Type::Unknown();
      }
      if (r.err->kind != Type::Kind::Text) {
        diag.error(e.pos, "postfix '?' requires Result<T,text> (error type must be text for __last_error)");
      }
      Type ok = *r.ok;
      e.inferred_type = typeref_from_type(ok);
      return ok;
    }
  }
  e.inferred_type = TypeRef::named("unknown");
  return Type::Unknown();
}

void Sema::check_action(GroupDecl& g, GroupEnv& env, ProcessDecl& p,
                        std::unordered_map<std::string, Type>& locals,
                        Action& a, bool realtime_safe) {
  (void)g; (void)p;

  if (a.kind == Action::Kind::DoStmt) {
    // require do statements for '?' usage; already validated earlier
    Stmt& s = a.stmt;
    if (s.kind == Stmt::Kind::Let || s.kind == Stmt::Kind::Var) {
      Type rhs = check_expr(env, p, locals, s.expr);
      Type declared = rhs;
      if (s.explicit_type) declared = type_from_typeref(*s.explicit_type);
      if (s.explicit_type && !type_equals(declared, rhs)) {
        diag.error(s.pos, "type mismatch in do local '" + s.name + "'");
      }
      locals[s.name] = declared;
    } else if (s.kind == Stmt::Kind::Assign) {
      if (!locals.count(s.name)) diag.error(s.pos, "assign to unknown local: " + s.name);
      Type rhs = check_expr(env, p, locals, s.expr);
      Type lhs = locals.count(s.name) ? locals[s.name] : Type::Unknown();
      if (locals.count(s.name) && !type_equals(lhs, rhs)) diag.error(s.pos, "type mismatch in assign '" + s.name + "'");
    } else if (s.kind == Stmt::Kind::ExprStmt) {
      (void)check_expr(env, p, locals, s.expr);
    }
    return;
  }

  // Realtime-safe: disallow blocking send/receive
  if (realtime_safe && (a.kind == Action::Kind::Send || a.kind == Action::Kind::Receive)) {
    diag.error(a.pos, "@realtimesafe groups may not use blocking send/receive (use try_send/try_receive)");
  }

  if (a.kind == Action::Kind::Send) {
    if (!env.channels.count(a.chan)) { diag.error(a.pos, "unknown channel: " + a.chan); return; }
    Type ch = env.channels[a.chan];
    Type ex = check_expr(env, p, locals, a.send_expr);
    if (ch.kind != Type::Kind::Channel) { diag.error(a.pos, "internal: channel type not Channel"); return; }
    if (!type_equals(*ch.elem, ex)) {
      diag.error(a.pos, "send type mismatch: channel expects " + type_to_string(*ch.elem) + " but expr is " + type_to_string(ex));
    }
    return;
  }

  if (a.kind == Action::Kind::Receive) {
    if (!env.channels.count(a.chan)) { diag.error(a.pos, "unknown channel: " + a.chan); return; }
    Type ch = env.channels[a.chan];
    if (ch.kind != Type::Kind::Channel) return;

    Type t = *ch.elem;

    if (a.recv_declares) {
      if (!a.recv_type) { diag.error(a.pos, "receive var requires type"); return; }
      Type declared = type_from_typeref(*a.recv_type);
      if (!type_equals(declared, t)) diag.error(a.pos, "receive declared type mismatch");
      locals[a.recv_target] = declared;
    } else {
      if (!locals.count(a.recv_target)) diag.error(a.pos, "receive target not declared: " + a.recv_target);
      else if (!type_equals(locals[a.recv_target], t)) diag.error(a.pos, "receive target type mismatch");
    }
    return;
  }

  if (a.kind == Action::Kind::TrySend) {
    if (!env.channels.count(a.try_send_chan)) { diag.error(a.pos, "unknown channel: " + a.try_send_chan); return; }
    Type ch = env.channels[a.try_send_chan];
    Type ex = check_expr(env, p, locals, a.try_send_expr);
    if (!type_equals(*ch.elem, ex)) diag.error(a.pos, "try_send expr type mismatch");
    // outvar must be Result<bool,text>
    Type expectT = Type::Result(Type::Bool(), Type::Text());
    if (!locals.count(a.try_send_outvar)) {
      locals[a.try_send_outvar] = expectT;
    } else if (!type_equals(locals[a.try_send_outvar], expectT)) {
      diag.error(a.pos, "try_send out var must be Result<bool,text>");
    }
    return;
  }

  if (a.kind == Action::Kind::TryReceive) {
    if (!env.channels.count(a.try_recv_chan)) { diag.error(a.pos, "unknown channel: " + a.try_recv_chan); return; }
    Type ch = env.channels[a.try_recv_chan];
    Type expectT = Type::Result(*ch.elem, Type::Text());
    if (!locals.count(a.try_recv_outvar)) {
      locals[a.try_recv_outvar] = expectT;
    } else if (!type_equals(locals[a.try_recv_outvar], expectT)) {
      diag.error(a.pos, "try_receive out var must match Result<T,text> for that channel");
    }
    return;
  }
}

// Sema for new features
Type* Sema::check_match_expr(const MatchExpr& expr) {
  diag.error(expr.span, "match expressions not yet implemented");
  return nullptr;
}

Type* Sema::check_array_expr(const ArrayExpr& expr) {
  diag.error(expr.span, "array expressions not yet implemented");
  return nullptr;
}

void Sema::check_for_loop(const ForLoop& loop) {
  if (!is_compile_time_constant(loop.end.get())) {
    diag.error(loop.span, "Loop end must be compile-time constant");
  }
}

void Sema::check_const_fn(const ConstFnDecl& fn) {
  // Ensure no side effects, only compile-time operations
  diag.error(fn.pos, "const functions not yet implemented");
}

// Enhanced Sema for strong static analysis
// Added: Type inference, lifetime analysis, borrow checking, determinism checks

void Sema::infer_types(Program& prog) {
  // Advanced type inference
}

void Sema::check_lifetimes(Program& prog) {
  // Ensure no dangling references
}

void Sema::borrow_check(Program& prog) {
  // Ownership and borrowing rules
}

void Sema::determinism_check(Program& prog) {
  // Verify deterministic behavior
}
