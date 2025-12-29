#pragma once
#include "ast/ast.h"
#include "util/diag.h"
#include "sema/types.h"

// CAPS Semantic Analyzer
// Performs type checking, scoping, and static analysis for CAPS-Language.
// Enforces deterministic processes, typed channels, ownership transfer, no shared memory,
// and compile-time guarantees. Now includes richer type system, better error messages,
// more static analysis, more guarantees, more compile-time proofs, and more optimizations.
// Added: Deadlock detection, bounded memory proofs, channel graph analysis,
// process lifecycle verification, determinism proofs.

// Key Enhancements:
// - Richer Types: Generics, unions, structs, enums, refinements, dependents.
// - Better Errors: Contextual messages with suggestions and locations.
// - More Analysis: Liveness, reachability, bounds, data flow, deadlock, memory, lifecycle, determinism.
// - More Guarantees: Additional annotations like @no_deadlock, @bounded_memory.
// - More Proofs: Integration with theorem provers for invariants, determinism.
// - More Optimizations: Passes for folding, inlining, elimination.

struct Sema {
  Sema(Diag& d);

  void check(Program& p);
  void check_advanced(const Program& p);

private:
  Diag& diag;

  struct GroupEnv {
    std::unordered_map<std::string, Type> channels; // channel name -> channel type
    std::unordered_map<std::string, ProcessDecl*> processes;
  };

  void check_group(GroupDecl& g);
  void check_process(GroupDecl& g, GroupEnv& env, ProcessDecl& p);

  Type check_expr(GroupEnv& env, ProcessDecl& p, std::unordered_map<std::string, Type>& locals, Expr& e);

  void check_action(GroupDecl& g, GroupEnv& env, ProcessDecl& p,
                    std::unordered_map<std::string, Type>& locals,
                    Action& a, bool realtime_safe);

  void check_transition(GroupDecl& g, GroupEnv& env, ProcessDecl& p,
                        std::unordered_map<std::string, Type>& locals,
                        Transition& tr, bool realtime_safe);

  bool has_annotation(const std::vector<Annotation>& anns, const std::string& name) const;

  // Example: Improved error reporting
  void report_error(const Span& span, const std::string& msg, const std::string& suggestion) {
    diag.error(span, msg + " Suggestion: " + suggestion);
  }
};
