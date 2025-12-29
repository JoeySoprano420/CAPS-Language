#include "backend/trace.h"
#include <sstream>

namespace caps {

static std::string buf_to_string(const std::deque<Value>& b) {
  std::ostringstream os;
  os << "[";
  for (size_t i=0;i<b.size();i++){
    if (i) os << ", ";
    os << to_string(b[i]);
  }
  os << "]";
  return os.str();
}

static const char* status_str(ProcStatus st) {
  switch (st) {
    case ProcStatus::Running: return "Running";
    case ProcStatus::Blocked: return "Blocked";
    case ProcStatus::Finished: return "Finished";
  }
  return "?";
}

void TextTrace::on_tick_begin(uint64_t tick) {
  out << "TICK " << tick << "\n";
}

void TextTrace::on_tick_end(uint64_t) {
  out << "END_TICK\n\n";
}

void TextTrace::on_process_step_begin(uint64_t, const std::string& proc, const std::string& state_before) {
  out << "  PROCESS_STEP " << proc << "\n";
  out << "    state_before: " << state_before << "\n";
  out << "    actions:\n";
}

void TextTrace::on_process_step_end(uint64_t, const std::string&, const std::string& state_after, ProcStatus st) {
  out << "    state_after: " << state_after << "\n";
  out << "    status_after: " << status_str(st) << "\n\n";
}

void TextTrace::on_assign(const std::string&, const std::string& var, const Value& before, const Value& after) {
  out << "      - kind: assign\n";
  out << "        var: " << var << "\n";
  out << "        before: " << to_string(before) << "\n";
  out << "        after: " << to_string(after) << "\n";
}

void TextTrace::on_send_begin(const std::string&, const std::string& chan, const Value& v, const std::deque<Value>& buf_before) {
  out << "      - kind: send\n";
  out << "        channel: " << chan << "\n";
  out << "        value: " << to_string(v) << "\n";
  out << "        channelbufferbefore: " << buf_to_string(buf_before) << "\n";
}

void TextTrace::on_send_end(const std::string&, const std::string&, const std::deque<Value>& buf_after) {
  out << "        channelbufferafter: " << buf_to_string(buf_after) << "\n";
}

void TextTrace::on_receive_begin(const std::string&, const std::string& chan, const std::deque<Value>& buf_before) {
  out << "      - kind: receive\n";
  out << "        channel: " << chan << "\n";
  out << "        channelbufferbefore: " << buf_to_string(buf_before) << "\n";
}

void TextTrace::on_receive_end(const std::string&, const std::string&, const Value& v, const std::deque<Value>& buf_after) {
  out << "        value: " << to_string(v) << "\n";
  out << "        channelbufferafter: " << buf_to_string(buf_after) << "\n";
}

void TextTrace::on_try_send(const std::string&, const std::string& chan, const Value& v, bool success, const std::deque<Value>& buf_after) {
  out << "      - kind: try_send\n";
  out << "        channel: " << chan << "\n";
  out << "        value: " << to_string(v) << "\n";
  out << "        success: " << (success ? "true":"false") << "\n";
  out << "        channelbufferafter: " << buf_to_string(buf_after) << "\n";
}

void TextTrace::on_try_receive(const std::string&, const std::string& chan, bool ok, const Value& v, const std::deque<Value>& buf_after) {
  out << "      - kind: try_receive\n";
  out << "        channel: " << chan << "\n";
  out << "        ok: " << (ok ? "true":"false") << "\n";
  out << "        value: " << to_string(v) << "\n";
  out << "        channelbufferafter: " << buf_to_string(buf_after) << "\n";
}

void TextTrace::on_block(const std::string&, const std::string& op, const std::string& chan, const std::string& reason) {
  out << "        blocked: true\n";
  out << "        op: " << op << "\n";
  out << "        channel: " << chan << "\n";
  out << "        reason: " << reason << "\n";
}

void TextTrace::on_transition_skipped(uint64_t, const std::string&, const std::string& reason) {
  out << "    transition:\n";
  out << "      kind: skipped\n";
  out << "      reason: " << reason << "\n";
}

void TextTrace::on_status(const std::string& status, const std::string& reason, const Runtime& rt) {
  out << "RUNTIME_STATUS\n";
  out << "  status: " << status << "\n";
  out << "  reason: " << reason << "\n";
  out << "  processes:\n";
  for (auto& kv : rt.procs) {
    out << "    " << kv.first << ": state=" << kv.second.state << " status=" << status_str(kv.second.status) << "\n";
  }
  out << "  channels:\n";
  for (auto& kv : rt.channels) {
    out << "    " << kv.first << ": buffer=" << buf_to_string(kv.second.buffer) << "\n";
  }
  out << "END_STATUS\n";
}

} // namespace caps
