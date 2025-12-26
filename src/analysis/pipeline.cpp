#include "analysis/pipeline.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>

static bool has_ann(const std::vector<Annotation>& anns, const std::string& n) {
  for (auto& a : anns) if (a.name == n) return true;
  return false;
}

void PipelineChecker::check_group_pipeline_safe(const GroupDecl& g) {
  if (!has_ann(g.annotations, "pipeline_safe")) return;

  // Map channel name -> {writers, readers}
  struct Use { std::unordered_set<std::string> writers; std::unordered_set<std::string> readers; };
  std::unordered_map<std::string, Use> uses;

  for (auto& c : g.channels) uses[c.name] = Use{};

  auto record_action = [&](const std::string& proc, const Action& a) {
    if (a.kind == Action::Kind::Send) {
      uses[a.chan].writers.insert(proc);
    } else if (a.kind == Action::Kind::Receive) {
      uses[a.chan].readers.insert(proc);
    } else if (a.kind == Action::Kind::TrySend) {
      uses[a.try_send_chan].writers.insert(proc);
    } else if (a.kind == Action::Kind::TryReceive) {
      uses[a.try_recv_chan].readers.insert(proc);
    }
  };

  // Collect use from all actions, including transition branch action lists
  for (auto& p : g.processes) {
    for (auto& ob : p.on_blocks) {
      for (auto& a : ob.actions) record_action(p.name, a);
      if (ob.transition.kind == Transition::Kind::IfElse) {
        for (auto& a : ob.transition.then_actions) record_action(p.name, a);
        for (auto& a : ob.transition.else_actions) record_action(p.name, a);
      }
    }
  }

  // Enforce single-writer/single-reader per channel
  for (auto& [ch, u] : uses) {
    if (u.writers.size() != 1 || u.readers.size() != 1) {
      diag.error(g.pos, "@pipeline_safe requires each channel to have exactly 1 writer and 1 reader: channel '" + ch + "'");
    }
  }

  // Build process dependency graph edges writer -> reader
  std::unordered_map<std::string, std::vector<std::string>> adj;
  std::unordered_map<std::string, int> indeg;
  for (auto& p : g.processes) { adj[p.name] = {}; indeg[p.name] = 0; }

  for (auto& [ch, u] : uses) {
    if (u.writers.size() == 1 && u.readers.size() == 1) {
      std::string w = *u.writers.begin();
      std::string r = *u.readers.begin();
      adj[w].push_back(r);
      indeg[r]++;
    }
  }

  // Kahn DAG check
  std::vector<std::string> q;
  q.reserve(indeg.size());
  for (auto& [p, d] : indeg) if (d == 0) q.push_back(p);

  std::vector<std::string> topo;
  topo.reserve(indeg.size());

  for (size_t i = 0; i < q.size(); i++) {
    std::string n = q[i];
    topo.push_back(n);
    for (auto& m : adj[n]) {
      indeg[m]--;
      if (indeg[m] == 0) q.push_back(m);
    }
  }

  if (topo.size() != g.processes.size()) {
    diag.error(g.pos, "@pipeline_safe topology has a cycle (not a DAG)");
    return;
  }

  // Schedule respects topo order: step index must be non-decreasing along edges
  std::unordered_map<std::string, int> step_index;
  for (int i = 0; i < (int)g.schedule.steps.size(); i++) step_index[g.schedule.steps[i]] = i;

  for (auto& [from, outs] : adj) {
    for (auto& to : outs) {
      if (step_index.count(from) && step_index.count(to)) {
        if (step_index[from] > step_index[to]) {
          diag.error(g.schedule.pos, "@pipeline_safe schedule violates topo order: '" + from + "' must be before '" + to + "'");
        }
      }
    }
  }
}
