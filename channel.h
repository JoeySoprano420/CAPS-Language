#pragma once
#include "backend/value.h"
#include <deque>
#include <string>

namespace caps {

struct Channel {
  std::string name;
  size_t capacity = 0; // 0 = synchronous (unbuffered)
  std::deque<Value> buffer;

  // Deterministic FIFO. For capacity==0, we treat it as "must pair send/recv in same tick step".
  // The scheduler implements pairing logic by checking recv-ready/send-ready; for simplicity,
  // we model cap==0 as "no buffer", send blocks if buffer is 'occupied' (never) and if no receiver step occurs.
  // In this reference runtime, cap==0 behaves like cap==0 buffer: send blocks unless a receiver is waiting.
};

} // namespace caps
