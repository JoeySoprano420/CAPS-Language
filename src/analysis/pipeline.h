#pragma once
#include "ast/ast.h"
#include "util/diag.h"
#include <string>
#include <vector>

struct PipelineEdge {
  std::string from_process;
  std::string channel;
  std::string to_process;
};

struct PipelineGraph {
  std::vector<PipelineEdge> edges; // Process -> Channel -> Process (collapsed into triple)
};

PipelineGraph build_pipeline_graph(const GroupDecl& g);

// DOT output for graphviz (digraph)
void dump_pipeline_graph_dot(std::ostream& os, const GroupDecl& g, const PipelineGraph& pg);

struct PipelineChecker {
  explicit PipelineChecker(Diag& d) : diag(d) {}
  void check_group_pipeline_safe(const GroupDecl& g);

private:
  Diag& diag;
};
