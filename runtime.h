#pragma once
#include "backend/ir.h"
#include "backend/channel.h"
#include "backend/value.h"
#include <unordered_map>

namespace caps {

enum class ProcStatus { Running, Blocked, Finished };

struct ProcessInstance {
  std::string name;
  const IRProcess* def = nullptr;

  std::string state;
  ProcStatus status = ProcStatus::Running;

  std::unordered_map<std::string, Value> locals;
  std::unordered_map<std::string, Value> outputs;

  // for Blocked: what we’re waiting on
  std::string blocked_chan;
  bool blocked_is_send = false;
};

struct Runtime {
  const IRGroup* group = nullptr;

  std::unordered_map<std::string, Channel> channels;
  std::unordered_map<std::string, ProcessInstance> procs;

  uint64_t tick = 0;
  size_t schedule_pos = 0;

  // deadlock detection
  uint64_t steps_since_progress = 0;
};

} // namespace caps
