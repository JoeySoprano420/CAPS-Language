#pragma once
#include "backend/runtime.h"
#include "backend/trace.h"

namespace caps {

enum class RunStatus { Completed, Deadlock, Running };

struct RunResult {
  RunStatus status = RunStatus::Running;
  std::string reason;
};

void init_runtime(Runtime& rt, const IRGroup& g);

RunResult run_group(Runtime& rt, TraceSink* trace, uint64_t max_ticks = 1'000'000);

} // namespace caps
