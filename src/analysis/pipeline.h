#pragma once
#include "ast/ast.h"
#include "util/diag.h"

struct PipelineChecker {
  explicit PipelineChecker(Diag& d) : diag(d) {}

  void check_group_pipeline_safe(const GroupDecl& g);

private:
  Diag& diag;
};
