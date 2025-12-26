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

static bool has_ann(const std::vector<Annotation>& anns, const std::string& n) {
  for (auto& a : anns) if (a.name == n) return true;
  return false;
}


enum class TopologyFormat { None, Dot, Text };

struct Options {
  bool dump_ast = false;
  TopologyFormat dump_topology = TopologyFormat::None;
  bool check_only = false;
  std::string input_file;
};


static void print_usage() {
  std::cerr <<
    "usage: caps_frontend [--dump-ast] [--dump-topology=dot|text] [--check-only] <file.caps>\n"
    "\n"
    "  --dump-ast             Print parsed+sema-mutated AST\n"
    "  --dump-topology=dot    Print @pipeline_safe topology as Graphviz DOT\n"
    "  --dump-topology=text   Print @pipeline_safe topology as human-readable text\n"
    "  --check-only           CI mode: diagnostics only; exit 0 on success, 2 on any error\n";
}


static bool parse_args(int argc, char** argv, Options& opt) {
  for (int i = 1; i < argc; i++) {
    std::string a = argv[i];

    if (a == "--dump-ast") { opt.dump_ast = true; continue; }

    if (a.rfind("--dump-topology", 0) == 0) {
      // Accept: --dump-topology=dot or --dump-topology=text
      auto eq = a.find('=');
      if (eq == std::string::npos) {
        std::cerr << "error: --dump-topology requires '=dot' or '=text'\n";
        return false;
      }
      std::string fmt = a.substr(eq + 1);
      if (fmt == "dot") opt.dump_topology = TopologyFormat::Dot;
      else if (fmt == "text") opt.dump_topology = TopologyFormat::Text;
      else {
        std::cerr << "error: unknown --dump-topology format: " << fmt << "\n";
        return false;
      }
      continue;
    }

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

  // --dump-topology (all @pipeline_safe groups)
  if (opt.dump_topology != TopologyFormat::None) {
    bool printed_any = false;

    for (auto& g : prog.groups) {
      if (!has_ann(g.annotations, "pipeline_safe")) continue;

      printed_any = true;
      auto tg = build_topology_graph(g);

      // Emit ambiguous warnings to stderr
      for (auto& w : tg.ambiguities) {
        std::cerr << "warning: group '" << g.name << "' channel '" << w.channel
                  << "' ambiguous (writers=" << w.writers.size()
                  << ", readers=" << w.readers.size() << ")\n";
      }

      if (opt.dump_topology == TopologyFormat::Dot) {
        dump_topology_dot(std::cout, g, tg);
        std::cout << "\n";
      } else {
        dump_topology_text(std::cout, g, tg);
        std::cout << "\n";
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
