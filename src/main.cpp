#include <iostream>
#include "util/str.h"
#include "util/diag.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "sema/sema.h"
#include "ir/lowering.h"
#include "pretty/pretty.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "usage: caps_frontend <file.caps>\n";
    return 1;
  }

  std::string path = argv[1];
  std::string src;
  try {
    src = read_file_to_string(path);
  } catch (const std::exception& e) {
    std::cerr << "read error: " << e.what() << "\n";
    return 1;
  }

  Diag diag;
  Lexer lex(src, diag);
  Parser parser(lex, diag);

  Program prog = parser.parse_program();

  Sema sema(diag);
  sema.check(prog);

  if (diag.has_errors()) {
    diag.print_all(std::cerr);
    return 2;
  }

  // Lower all groups to IR and print
  Lowering lower;
  for (auto& g : prog.groups) {
    IRGroup irg = lower.lower_group(g);
    print_ir(std::cout, irg);
  }

  return 0;
}
