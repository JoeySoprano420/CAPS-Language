#include "pretty/pretty.h"

static void print_actions(std::ostream& os, const std::vector<IRAction>& acts, int indent) {
  auto pad = [&](int n){ for(int i=0;i<n;i++) os << "  "; };
  for (auto& a : acts) {
    pad(indent);
    switch (a.kind) {
      case IRAction::Kind::Assign:
        os << "Assign " << a.dst << " = <expr>\n";
        break;
      case IRAction::Kind::Send:
        os << "Send <expr> -> " << a.chan << "\n";
        break;
      case IRAction::Kind::Receive:
        os << "Receive " << a.chan << " -> " << a.dst << "\n";
        break;
      case IRAction::Kind::TrySend:
        os << "TrySend <expr> -> " << a.chan << " -> " << a.try_send_out << "\n";
        break;
      case IRAction::Kind::TryReceive:
        os << "TryReceive " << a.chan << " -> " << a.try_recv_out << "\n";
        break;
      case IRAction::Kind::TryUnwrapAssign:
        os << "TryUnwrapAssign " << a.unwrap_dst << " = (Result)?  on_fail: state=" << a.unwrap_error_state
           << " write " << a.unwrap_last_error << "\n";
        break;
    }
  }
}

void print_ir(std::ostream& os, const IRGroup& g) {
  os << "IRGroup " << g.name << "\n";
  os << "  Schedule:";
  for (auto& s : g.schedule_steps) os << " " << s;
  os << "\n\n";

  for (auto& p : g.processes) {
    os << "  IRProcess " << p.name << "\n";
    for (auto& st : p.states) {
      os << "    State " << st.name << "\n";
      os << "      Actions:\n";
      print_actions(os, st.actions, 4);
      os << "      Transition:\n";
      if (st.transition.kind == IRTransition::Kind::Unconditional) {
        os << "        -> " << st.transition.to << "\n";
      } else {
        os << "        if <cond>\n";
        os << "          then:\n";
        print_actions(os, st.transition.then_actions, 6);
        os << "          -> " << st.transition.then_to << "\n";
        os << "          else:\n";
        print_actions(os, st.transition.else_actions, 6);
        os << "          -> " << st.transition.else_to << "\n";
      }
    }
    os << "\n";
  }
}
