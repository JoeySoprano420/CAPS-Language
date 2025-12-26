#include "pretty/ast_dump.h"
#include <string>

static void ind(std::ostream& os, int n) { for (int i = 0; i < n; i++) os << "  "; }

static void dump_typeref(std::ostream& os, const TypeRef& t) {
  if (t.is_channel) {
    os << "channel<";
    if (!t.args.empty()) dump_typeref(os, t.args[0]);
    os << ";" << t.channel_capacity << ">";
    return;
  }
  os << t.name;
  if (!t.args.empty()) {
    os << "<";
    for (size_t i = 0; i < t.args.size(); i++) {
      if (i) os << ",";
      dump_typeref(os, t.args[i]);
    }
    os << ">";
  }
}

static void dump_expr(std::ostream& os, const Expr& e);

static void dump_expr(std::ostream& os, const Expr& e) {
  switch (e.kind) {
    case Expr::Kind::Ident: os << e.text; return;
    case Expr::Kind::IntLit: os << e.text; return;
    case Expr::Kind::RealLit: os << e.text; return;
    case Expr::Kind::TextLit: os << "\"" << e.text << "\""; return;
    case Expr::Kind::Binary:
      os << "(";
      dump_expr(os, e.args[0]);
      os << " " << e.text << " ";
      dump_expr(os, e.args[1]);
      os << ")";
      return;
    case Expr::Kind::Call:
      // args[0] = callee
      dump_expr(os, e.args[0]);
      os << "(";
      for (size_t i = 1; i < e.args.size(); i++) {
        if (i != 1) os << ", ";
        dump_expr(os, e.args[i]);
      }
      os << ")";
      return;
    case Expr::Kind::Try:
      dump_expr(os, e.args[0]);
      os << "?";
      return;
  }
}

static void dump_stmt(std::ostream& os, const Stmt& s, int depth) {
  ind(os, depth);
  switch (s.kind) {
    case Stmt::Kind::Let: os << "let "; break;
    case Stmt::Kind::Var: os << "var "; break;
    case Stmt::Kind::Assign: os << "assign "; break;
    case Stmt::Kind::Return: os << "return "; break;
    case Stmt::Kind::ExprStmt: os << "expr "; break;
  }
  if (!s.name.empty()) os << s.name;
  if (s.explicit_type) {
    os << ":";
    dump_typeref(os, *s.explicit_type);
  }
  if (s.kind != Stmt::Kind::Return && s.kind != Stmt::Kind::ExprStmt) os << " = ";
  if (s.kind == Stmt::Kind::Return) {
    dump_expr(os, s.expr);
  } else if (s.kind == Stmt::Kind::ExprStmt) {
    dump_expr(os, s.expr);
  } else {
    dump_expr(os, s.expr);
  }
  os << "\n";
}

static void dump_action(std::ostream& os, const Action& a, int depth) {
  ind(os, depth);
  switch (a.kind) {
    case Action::Kind::DoStmt:
      os << "do ";
      // print stmt inline
      if (a.stmt.kind == Stmt::Kind::Let) os << "let ";
      else if (a.stmt.kind == Stmt::Kind::Var) os << "var ";
      else if (a.stmt.kind == Stmt::Kind::Assign) os << "";
      else if (a.stmt.kind == Stmt::Kind::Return) os << "return ";
      else os << "";
      if (!a.stmt.name.empty()) os << a.stmt.name;
      if (a.stmt.explicit_type) { os << ":"; dump_typeref(os, *a.stmt.explicit_type); }
      if (a.stmt.kind == Stmt::Kind::Let || a.stmt.kind == Stmt::Kind::Var || a.stmt.kind == Stmt::Kind::Assign) {
        os << " = ";
      } else if (a.stmt.kind == Stmt::Kind::Return) {
        os << " ";
      } else {
        os << " ";
      }
      dump_expr(os, a.stmt.expr);
      os << "\n";
      return;

    case Action::Kind::Send:
      os << "send ";
      dump_expr(os, a.send_expr);
      os << " -> " << a.chan << "\n";
      return;

    case Action::Kind::Receive:
      os << "receive " << a.chan << " -> ";
      if (a.recv_declares) {
        os << "var " << a.recv_target << ":";
        if (a.recv_type) dump_typeref(os, *a.recv_type);
      } else {
        os << a.recv_target;
      }
      os << "\n";
      return;

    case Action::Kind::TrySend:
      os << "try_send ";
      dump_expr(os, a.try_send_expr);
      os << " -> " << a.try_send_chan << " -> " << a.try_send_outvar << "\n";
      return;

    case Action::Kind::TryReceive:
      os << "try_receive " << a.try_recv_chan << " -> " << a.try_recv_outvar << "\n";
      return;
  }
}

static void dump_transition(std::ostream& os, const Transition& tr, int depth) {
  ind(os, depth);
  if (tr.kind == Transition::Kind::Unconditional) {
    os << "-> " << tr.to_state << "\n";
    return;
  }
  os << "if ";
  dump_expr(os, tr.cond);
  os << "\n";
  ind(os, depth + 1);
  os << "then_actions:\n";
  for (auto& a : tr.then_actions) dump_action(os, a, depth + 2);
  ind(os, depth + 1);
  os << "-> " << tr.then_state << "\n";

  ind(os, depth + 1);
  os << "else_actions:\n";
  for (auto& a : tr.else_actions) dump_action(os, a, depth + 2);
  ind(os, depth + 1);
  os << "-> " << tr.else_state << "\n";
}

static void dump_annotations(std::ostream& os, const std::vector<Annotation>& anns, int depth) {
  for (auto& a : anns) {
    ind(os, depth);
    os << "@" << a.name;
    if (!a.args.empty()) {
      os << "(";
      for (size_t i = 0; i < a.args.size(); i++) {
        if (i) os << ", ";
        os << a.args[i];
      }
      os << ")";
    }
    os << "\n";
  }
}

void dump_ast(std::ostream& os, const Program& p) {
  os << "Program\n";
  ind(os, 1);
  os << "module " << p.module.name << "\n";

  for (auto& g : p.groups) {
    ind(os, 1);
    os << "group " << g.name << "\n";
    dump_annotations(os, g.annotations, 2);

    ind(os, 2); os << "channels:\n";
    for (auto& c : g.channels) {
      ind(os, 3);
      os << "channel<";
      dump_typeref(os, c.elem_type);
      os << ";" << c.capacity << "> " << c.name << "\n";
    }

    ind(os, 2); os << "processes:\n";
    for (auto& pr : g.processes) {
      ind(os, 3);
      os << "process " << pr.name << "\n";
      dump_annotations(os, pr.annotations, 4);

      ind(os, 4); os << "inputs(";
      for (size_t i = 0; i < pr.inputs.size(); i++) {
        if (i) os << ", ";
        os << pr.inputs[i].name << ":";
        dump_typeref(os, pr.inputs[i].type);
      }
      os << ")\n";

      ind(os, 4); os << "outputs(";
      for (size_t i = 0; i < pr.outputs.size(); i++) {
        if (i) os << ", ";
        os << pr.outputs[i].name << ":";
        dump_typeref(os, pr.outputs[i].type);
      }
      os << ")\n";

      ind(os, 4); os << "states:";
      for (auto& s : pr.states) os << " " << s;
      os << "\n";

      ind(os, 4); os << "locals:\n";
      for (auto& st : pr.locals) dump_stmt(os, st, 5);

      ind(os, 4); os << "on_blocks:\n";
      for (auto& ob : pr.on_blocks) {
        ind(os, 5); os << "on " << ob.state_name << "\n";
        ind(os, 6); os << "actions:\n";
        for (auto& a : ob.actions) dump_action(os, a, 7);
        ind(os, 6); os << "transition:\n";
        dump_transition(os, ob.transition, 7);
      }
    }

    ind(os, 2); os << "schedule:\n";
    ind(os, 3); os << "steps:";
    for (auto& s : g.schedule.steps) os << " " << s;
    os << "\n";
    ind(os, 3); os << "repeat: " << (g.schedule.repeat ? "true" : "false") << "\n";
  }
}
