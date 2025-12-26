#include <iostream>
#include <string>
#include <vector>

#include "util/str.h"
#include "util/diag.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "sema/sema.h"
#include "ir/lowering.h"
#include "pretty/pretty.h"
#include "pretty/ast_dump.h"
#include "analysis/pipeline.h"

struct Options {
  bool dump_ast = false;
  bool dump_topology = false;
  bool check_only = false;
  std::string input_file;
};

static void print_usage() {
  std::cerr <<
    "usage: caps_frontend [--dump-ast] [--dump-topology] [--check-only] <file.caps>\n"
    "\n"
    "  --dump-ast       Print parsed+sema-mutated AST (includes injected __last_error if '?' used)\n"
    "  --dump-topology  Print @pipeline_safe topology as Graphviz DOT to stdout\n"
    "  --check-only     CI mode: print diagnostics; exit 0 on success, 2 on any error\n";
}

static bool parse_args(int argc, char** argv, Options& opt) {
  for (int i = 1; i < argc; i++) {
    std::string a = argv[i];
    if (a == "--dump-ast") { opt.dump_ast = true; continue; }
    if (a == "--dump-topology") { opt.dump_topology = true; continue; }
    if (a == "--check-only") { opt.check_only = true; continue; }
    if (!a.empty() && a[0] == '-') {
      std::cerr << "unknown option: " << a << "\n";
      return false;
    }
    opt.input_file = a;
  }
  return !opt.input_file.empty();
}

int main(int argc, char** argv) {
  Options opt;
  if (!parse_args(argc, argv, opt)) {
    print_usage();
    return 1;
  }

  std::string src;
  try {
    src = read_file_to_string(opt.input_file);
  } catch (const std::exception& e) {
    std::cerr << "read error: " << e.what() << "\n";
    return 1;
  }

  Diag diag;
  Lexer lex(src, diag);
  Parser parser(lex, diag);

  Program prog = parser.parse_program();

  // Sema (also runs pipeline checks)
  Sema sema(diag);
  sema.check(prog);

  if (diag.has_errors()) {
    diag.print_all(std::cerr);
    return 2; // CI-compatible error code
  }

  // In check-only mode: succeed without emitting anything (unless user asked for dumps)
  // (This makes it friendly for CI logs.)
  if (opt.check_only && !opt.dump_ast && !opt.dump_topology) {
    return 0;
  }

  // --dump-ast
  if (opt.dump_ast) {
    dump_ast(std::cout, prog);
    if (!opt.dump_topology) std::cout << "\n";
  }

  // --dump-topology
  if (opt.dump_topology) {
    bool printed_any = false;
    for (auto& g : prog.groups) {
      if (has_ann(g.annotations, "pipeline_safe")) {
        auto pg = build_pipeline_graph(g);
        dump_pipeline_graph_dot(std::cout, g, pg);
        printed_any = true;
        break; // default: first pipeline_safe group
      }
    }
    if (!printed_any) {
      std::cerr << "warning: --dump-topology requested but no @pipeline_safe group found\n";
    }
  }

  // Default output (IR) if not check-only
  if (!opt.check_only) {
    Lowering lower;
    for (auto& g : prog.groups) {
      IRGroup irg = lower.lower_group(g);
      print_ir(std::cout, irg);
    }
  }

  return 0;
}
