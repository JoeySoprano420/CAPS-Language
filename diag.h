#pragma once
#include <string>
#include <vector>
#include <iostream>

struct SourcePos {
  int line = 1;
  int col = 1;
};

struct Diagnostic {
  enum class Kind { Error, Warning } kind;
  SourcePos pos;
  std::string msg;
};

struct Diag {
  std::vector<Diagnostic> diags;

  void error(SourcePos p, const std::string& m) {
    diags.push_back({Diagnostic::Kind::Error, p, m});
  }

  void warning(SourcePos p, const std::string& m) {
    diags.push_back({Diagnostic::Kind::Warning, p, m});
  }

  bool has_errors() const {
    for (auto& d : diags) if (d.kind == Diagnostic::Kind::Error) return true;
    return false;
  }

  void print_all(std::ostream& os) const {
    for (auto& d : diags) {
      os << (d.kind == Diagnostic::Kind::Error ? "error" : "warning")
         << " @" << d.pos.line << ":" << d.pos.col << " - " << d.msg << "\n";
    }
  }
};
