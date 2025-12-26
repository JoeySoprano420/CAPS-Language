#pragma once
#include "ast/ast.h"
#include "ir/ir.h"

struct Lowering {
  IRGroup lower_group(const GroupDecl& g);

private:
  IRProcess lower_process(const ProcessDecl& p);
  IRState lower_on_block(const OnBlock& ob);

  void lower_stmt_as_actions(const Stmt& st, std::vector<IRAction>& out);
  void lower_action(const Action& a, std::vector<IRAction>& out);
};
