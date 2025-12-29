#pragma once
#include "backend/value.h"
#include "backend/channel.h"
#include "backend/runtime.h"
#include <deque>
#include <ostream>
#include <string>
#include <vector>

namespace caps {

struct TraceSink {
  virtual ~TraceSink() = default;

  virtual void on_tick_begin(uint64_t tick) = 0;
  virtual void on_tick_end(uint64_t tick) = 0;

  virtual void on_process_step_begin(uint64_t tick, const std::string& proc, const std::string& state_before) = 0;
  virtual void on_process_step_end(uint64_t tick, const std::string& proc, const std::string& state_after, ProcStatus st) = 0;

  virtual void on_assign(const std::string& proc, const std::string& var, const Value& before, const Value& after) = 0;

  virtual void on_send_begin(const std::string& proc, const std::string& chan, const Value& v, const std::deque<Value>& buf_before) = 0;
  virtual void on_send_end(const std::string& proc, const std::string& chan, const std::deque<Value>& buf_after) = 0;

  virtual void on_receive_begin(const std::string& proc, const std::string& chan, const std::deque<Value>& buf_before) = 0;
  virtual void on_receive_end(const std::string& proc, const std::string& chan, const Value& v, const std::deque<Value>& buf_after) = 0;

  virtual void on_try_send(const std::string& proc, const std::string& chan, const Value& v, bool success, const std::deque<Value>& buf_after) = 0;
  virtual void on_try_receive(const std::string& proc, const std::string& chan, bool ok, const Value& v, const std::deque<Value>& buf_after) = 0;

  virtual void on_block(const std::string& proc, const std::string& op, const std::string& chan, const std::string& reason) = 0;

  virtual void on_transition_skipped(uint64_t tick, const std::string& proc, const std::string& reason) = 0;

  virtual void on_status(const std::string& status, const std::string& reason, const Runtime& rt) = 0;
};

struct TextTrace : TraceSink {
  explicit TextTrace(std::ostream& os) : out(os) {}

  void on_tick_begin(uint64_t tick) override;
  void on_tick_end(uint64_t tick) override;

  void on_process_step_begin(uint64_t tick, const std::string& proc, const std::string& state_before) override;
  void on_process_step_end(uint64_t tick, const std::string& proc, const std::string& state_after, ProcStatus st) override;

  void on_assign(const std::string& proc, const std::string& var, const Value& before, const Value& after) override;

  void on_send_begin(const std::string& proc, const std::string& chan, const Value& v, const std::deque<Value>& buf_before) override;
  void on_send_end(const std::string& proc, const std::string& chan, const std::deque<Value>& buf_after) override;

  void on_receive_begin(const std::string& proc, const std::string& chan, const std::deque<Value>& buf_before) override;
  void on_receive_end(const std::string& proc, const std::string& chan, const Value& v, const std::deque<Value>& buf_after) override;

  void on_try_send(const std::string& proc, const std::string& chan, const Value& v, bool success, const std::deque<Value>& buf_after) override;
  void on_try_receive(const std::string& proc, const std::string& chan, bool ok, const Value& v, const std::deque<Value>& buf_after) override;

  void on_block(const std::string& proc, const std::string& op, const std::string& chan, const std::string& reason) override;

  void on_transition_skipped(uint64_t tick, const std::string& proc, const std::string& reason) override;

  void on_status(const std::string& status, const std::string& reason, const Runtime& rt) override;

private:
  std::ostream& out;
};

} // namespace caps
