#include "backend/scheduler.h"
#include "backend/exec.h"
#include <stdexcept>

namespace caps {

static bool all_finished(const Runtime& rt) {
  for (auto& kv : rt.procs) {
    if (kv.second.status != ProcStatus::Finished) return false;
  }
  return true;
}

static bool any_running(const Runtime& rt) {
  for (auto& kv : rt.procs) {
    if (kv.second.status == ProcStatus::Running) return true;
  }
  return false;
}

static bool any_progress_possible(const Runtime& rt) {
  // If there is at least one Running, progress is possible.
  if (any_running(rt)) return true;

  // If all are Blocked, progress is possible only if some channel condition changed.
  // In this deterministic scheduled model, if nobody is Running, nothing can change.
  return false;
}

void init_runtime(Runtime& rt, const IRGroup& g) {
  rt.group = &g;
  rt.channels.clear();
  rt.procs.clear();
  rt.tick = 0;
  rt.schedule_pos = 0;
  rt.steps_since_progress = 0;

  // Channels
  for (auto& c : g.channels) {
    Channel ch;
    ch.name = c.name;
    ch.capacity = c.capacity;
    rt.channels[ch.name] = std::move(ch);
  }

  // Processes
  for (auto& p : g.processes) {
    ProcessInstance pi;
    pi.name = p.name;
    pi.def = &p;
    pi.state = p.initial_state;
    pi.status = ProcStatus::Running;

    // init locals + outputs as unset
    for (auto& n : p.local_names) pi.locals[n] = Value::unset();
    for (auto& n : p.output_names) pi.outputs[n] = Value::unset();

    // special: __last_error always present if user wants it; safe to always include
    pi.locals["__last_error"] = Value::s("");

    rt.procs[pi.name] = std::move(pi);
  }

  // Trace header/topology is emitted by TraceSink implementation if desired.
}

RunResult run_group(Runtime& rt, TraceSink* trace, uint64_t max_ticks) {
  if (!rt.group) throw std::runtime_error("runtime not initialized");

  auto& g = *rt.group;

  for (uint64_t t = 0; t < max_ticks; t++) {
    rt.tick++;

    if (trace) trace->on_tick_begin(rt.tick);

    bool progress_this_tick = false;

    // One schedule cycle = one tick
    for (auto& pname : g.schedule.steps) {
      auto it = rt.procs.find(pname);
      if (it == rt.procs.end()) throw std::runtime_error("schedule references unknown process: " + pname);

      auto& p = it->second;

      if (p.status == ProcStatus::Running) {
        bool progressed = step_process_once(rt, p, trace);
        progress_this_tick = progress_this_tick || progressed;
      }
    }

    if (trace) trace->on_tick_end(rt.tick);

    if (all_finished(rt)) {
      if (trace) trace->on_status("Completed", "allprocessesterminal", rt);
      return {RunStatus::Completed, "allprocessesterminal"};
    }

    if (!progress_this_tick) {
      // No one stepped (all blocked or finished)
      bool any_blocked = false;
      bool all_done_or_blocked = true;
      for (auto& kv : rt.procs) {
        if (kv.second.status == ProcStatus::Blocked) any_blocked = true;
        if (kv.second.status == ProcStatus::Running) all_done_or_blocked = false;
      }

      if (any_blocked && all_done_or_blocked) {
        if (trace) trace->on_status("Deadlock", "allprocessesblockednoprogress", rt);
        return {RunStatus::Deadlock, "allprocessesblockednoprogress"};
      }
    }
  }

  if (trace) trace->on_status("Deadlock", "maxticks_exceeded", rt);
  return {RunStatus::Deadlock, "maxticks_exceeded"};
}

} // namespace caps
