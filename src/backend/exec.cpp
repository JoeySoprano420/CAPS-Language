#include "backend/exec.h"
#include "backend/eval.h"
#include "backend/result.h"
#include <stdexcept>

namespace caps {

static void trace_channel(TraceSink* t, const std::string& ch, const Channel& c);

bool exec_action(Runtime& rt, ProcessInstance& p, const IRAction& a, TraceSink* trace) {
  using K = IRAction::Kind;

  if (p.status != ProcStatus::Running) return false;

  auto block_on = [&](const std::string& ch, bool is_send) {
    p.status = ProcStatus::Blocked;
    p.blocked_chan = ch;
    p.blocked_is_send = is_send;
  };

  switch (a.kind) {
    case K::Assign: {
      Value v = eval_expr(rt, &p, a.expr);
      auto before_it = p.locals.find(a.dst);
      Value before = (before_it == p.locals.end()) ? Value::unset() : before_it->second;
      p.locals[a.dst] = v;

      if (trace) trace->on_assign(p.name, a.dst, before, v);
      return false;
    }

    case K::Send: {
      auto it = rt.channels.find(a.chan);
      if (it == rt.channels.end()) throw std::runtime_error("send: unknown channel " + a.chan);

      Channel& c = it->second;
      Value v = eval_expr(rt, &p, a.expr);

      if (trace) trace->on_send_begin(p.name, a.chan, v, c.buffer);

      // Blocking rule:
      // - buffered: block if full
      // - unbuffered (cap=0): also block unless a receiver is already blocked on same channel
      if (c.capacity == 0) {
        // rendezvous: must have receiver waiting
        bool found_waiting_recv = false;
        for (auto& kv : rt.procs) {
          auto& q = kv.second;
          if (q.status == ProcStatus::Blocked && !q.blocked_is_send && q.blocked_chan == a.chan) {
            // deliver directly
            q.locals[q.blocked_chan + ".__recv_value"] = v; // internal mailbox
            q.status = ProcStatus::Running;
            q.blocked_chan.clear();
            found_waiting_recv = true;
            break;
          }
        }
        if (!found_waiting_recv) {
          if (trace) trace->on_block(p.name, "send", a.chan, "unbuffered_no_receiver");
          block_on(a.chan, true);
          return true;
        }
        if (trace) trace->on_send_end(p.name, a.chan, c.buffer);
        return false;
      }

      if (c.buffer.size() >= c.capacity) {
        if (trace) trace->on_block(p.name, "send", a.chan, "channel_full");
        block_on(a.chan, true);
        return true;
      }

      c.buffer.push_back(v);
      if (trace) trace->on_send_end(p.name, a.chan, c.buffer);
      return false;
    }

    case K::Receive: {
      auto it = rt.channels.find(a.chan);
      if (it == rt.channels.end()) throw std::runtime_error("receive: unknown channel " + a.chan);

      Channel& c = it->second;
      if (trace) trace->on_receive_begin(p.name, a.chan, c.buffer);

      if (c.capacity == 0) {
        // unbuffered rendezvous: sender must be blocked waiting
        // sender stores nothing; we look for any sender blocked on this channel
        bool found_sender = false;
        Value v = Value::unset();
        for (auto& kv : rt.procs) {
          auto& q = kv.second;
          if (q.status == ProcStatus::Blocked && q.blocked_is_send && q.blocked_chan == a.chan) {
            // can't extract value from sender in this simple model unless sender also had mailbox;
            // therefore in this reference runtime, we model unbuffered send delivering by waking receiver in send().
            // If we reach here, no pre-delivered value.
            found_sender = false;
            break;
          }
        }

        // see if send() already deposited value in internal mailbox
        auto mb = p.locals.find(a.chan + ".__recv_value");
        if (mb != p.locals.end()) {
          v = mb->second;
          p.locals.erase(mb);
          p.locals[a.dst] = v;
          if (trace) trace->on_receive_end(p.name, a.chan, v, c.buffer);
          return false;
        }

        if (!found_sender) {
          if (trace) trace->on_block(p.name, "receive", a.chan, "unbuffered_no_value");
          block_on(a.chan, false);
          return true;
        }
        // unreachable in this model
      }

      if (c.buffer.empty()) {
        if (trace) trace->on_block(p.name, "receive", a.chan, "channel_empty");
        block_on(a.chan, false);
        return true;
      }

      Value v = c.buffer.front();
      c.buffer.pop_front();

      p.locals[a.dst] = v;
      if (trace) trace->on_receive_end(p.name, a.chan, v, c.buffer);
      return false;
    }

    case K::TrySend: {
      auto it = rt.channels.find(a.chan);
      if (it == rt.channels.end()) throw std::runtime_error("try_send: unknown channel " + a.chan);
      Channel& c = it->second;

      Value v = eval_expr(rt, &p, a.expr);
      bool success = false;

      if (c.capacity == 0) {
        // unbuffered: succeed only if receiver already blocked
        for (auto& kv : rt.procs) {
          auto& q = kv.second;
          if (q.status == ProcStatus::Blocked && !q.blocked_is_send && q.blocked_chan == a.chan) {
            q.locals[q.blocked_chan + ".__recv_value"] = v;
            q.status = ProcStatus::Running;
            q.blocked_chan.clear();
            success = true;
            break;
          }
        }
      } else {
        if (c.buffer.size() < c.capacity) {
          c.buffer.push_back(v);
          success = true;
        }
      }

      // Result<bool,text>: ok=true always; value indicates success
      p.locals[a.dst] = make_result_ok(Value::b(success));
      if (trace) trace->on_try_send(p.name, a.chan, v, success, c.buffer);
      return false;
    }

    case K::TryReceive: {
      auto it = rt.channels.find(a.chan);
      if (it == rt.channels.end()) throw std::runtime_error("try_receive: unknown channel " + a.chan);
      Channel& c = it->second;

      if (c.capacity == 0) {
        // unbuffered: check mailbox
        auto mb = p.locals.find(a.chan + ".__recv_value");
        if (mb != p.locals.end()) {
          Value v = mb->second;
          p.locals.erase(mb);
          p.locals[a.dst] = make_result_ok(v);
          if (trace) trace->on_try_receive(p.name, a.chan, true, v, c.buffer);
          return false;
        }
        p.locals[a.dst] = make_result_err_text("empty");
        if (trace) trace->on_try_receive(p.name, a.chan, false, Value::unset(), c.buffer);
        return false;
      }

      if (c.buffer.empty()) {
        p.locals[a.dst] = make_result_err_text("empty");
        if (trace) trace->on_try_receive(p.name, a.chan, false, Value::unset(), c.buffer);
        return false;
      }

      Value v = c.buffer.front();
      c.buffer.pop_front();
      p.locals[a.dst] = make_result_ok(v);
      if (trace) trace->on_try_receive(p.name, a.chan, true, v, c.buffer);
      return false;
    }
  }

  throw std::runtime_error("unhandled action kind");
}

static const IRState& get_state(const IRProcess& def, const std::string& name) {
  auto it = def.states.find(name);
  if (it == def.states.end()) throw std::runtime_error("unknown state: " + name);
  return it->second;
}

bool step_process_once(Runtime& rt, ProcessInstance& p, TraceSink* trace) {
  if (p.status != ProcStatus::Running) return false;

  const IRProcess& def = *p.def;
  const IRState& st = get_state(def, p.state);

  if (trace) trace->on_process_step_begin(rt.tick, p.name, p.state);

  // execute base actions
  for (auto& a : st.actions) {
    bool blocked = exec_action(rt, p, a, trace);
    if (blocked) {
      if (trace) trace->on_process_step_end(rt.tick, p.name, p.state, p.status);
      return true; // action executed and blocked event emitted
    }
  }

  // transition evaluation + branch action lists
  std::string next = p.state;

  if (st.transition.kind == IRTransition::Kind::Goto) {
    next = st.transition.to_state;
  } else {
    Value c = eval_expr(rt, &p, st.transition.cond);
    bool cond = as_bool(c);

    auto& acts = cond ? st.transition.then_actions : st.transition.else_actions;
    auto& target = cond ? st.transition.then_state : st.transition.else_state;

    for (auto& a : acts) {
      bool blocked = exec_action(rt, p, a, trace);
      if (blocked) {
        // if blocked inside branch actions, do not transition this step
        if (trace) trace->on_transition_skipped(rt.tick, p.name, "blocked_in_branch_actions");
        if (trace) trace->on_process_step_end(rt.tick, p.name, p.state, p.status);
        return true;
      }
    }

    next = target;
  }

  // apply state change
  p.state = next;

  // if terminal, finished
  const IRState& nextst = get_state(def, p.state);
  if (nextst.terminal) p.status = ProcStatus::Finished;

  if (trace) trace->on_process_step_end(rt.tick, p.name, p.state, p.status);
  return true;
}

} // namespace caps
