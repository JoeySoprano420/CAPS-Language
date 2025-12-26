#pragma once
#include "backend/runtime.h"
#include "backend/trace.h"

namespace caps {

// Executes one action. Returns true if the process blocked.
bool exec_action(Runtime& rt, ProcessInstance& p, const IRAction& a, TraceSink* trace);

// Executes a single state "step": actions + transition (with branch action lists).
// Returns true if progress happened (state changed or action executed non-trivially).
bool step_process_once(Runtime& rt, ProcessInstance& p, TraceSink* trace);

} // namespace caps
