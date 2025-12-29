#pragma once
#include "ast/ast.h"
#include "util/diag.h"
#include <ostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// CAPS Pipeline Analysis Header
// Advanced static analysis for CAPS-Language.
// Includes deadlock detection, bounded memory proofs, channel graph analysis,
// process lifecycle verification, and determinism proofs.

struct ChannelUse {
  std::string channel;
  std::unordered_set<std::string> writers;
  std::unordered_set<std::string> readers;
};

struct TopologyEdge {
  std::string from_process;
  std::string channel;
  std::string to_process;
  bool dashed = false;          // dashed if ambiguous
  std::string note;             // optional annotation for output
};

struct TopologyGraph {
  // Collapsed triples: Process -> Channel -> Process (stored as a single triple per from->to via channel)
  std::vector<TopologyEdge> edges;

  // A flat per-channel use summary for diagnostics / text format.
  std::vector<ChannelUse> uses;

  // Channels that violate single-writer/single-reader.
  std::vector<ChannelUse> ambiguities;

  // New fields for advanced analysis
  std::vector<std::string> deadlocks;
  std::unordered_map<std::string, size_t> memory_bounds;
  // New: Graph representation
  std::unordered_map<std::string, std::vector<std::string>> channel_graph;
};

TopologyGraph build_topology_graph(const GroupDecl& g);

// Output formats
void dump_topology_dot(std::ostream& os, const GroupDecl& g, const TopologyGraph& tg);
void dump_topology_text(std::ostream& os, const GroupDecl& g, const TopologyGraph& tg);

struct PipelineChecker {
  explicit PipelineChecker(Diag& d) : diag(d) {}
  void check_group_pipeline_safe(const GroupDecl& g);

private:
  Diag& diag;
};

// New analysis functions
bool detect_deadlocks(const GroupDecl& group, Diag& diag);
bool prove_bounded_memory(const GroupDecl& group, Diag& diag);
void analyze_channel_graph(const GroupDecl& group, Diag& diag);
bool verify_lifecycles(const GroupDecl& group, Diag& diag);
bool prove_determinism(const GroupDecl& group, Diag& diag);
