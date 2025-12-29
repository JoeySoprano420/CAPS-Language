#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

// CAPS Formatter Tool
// Formats CAPS source code with advanced rules.

struct FormatOptions {
  int indent_size = 2;
  bool align_assignments = true;
  bool sort_imports = false;
  bool remove_trailing_whitespace = true;
};

std::string format_code(const std::string& content, const FormatOptions& opts) {
  std::string formatted;
  int indent = 0;
  bool in_string = false;
  bool in_comment = false;

  for (size_t i = 0; i < content.size(); ++i) {
    char c = content[i];
    char next = (i + 1 < content.size()) ? content[i + 1] : '\0';

    // Handle strings
    if (c == '"' && !in_comment) {
      in_string = !in_string;
    }

    // Handle comments
    if (c == '/' && next == '/' && !in_string) {
      in_comment = true;
    }
    if (c == '\n') {
      in_comment = false;
    }

    if (!in_string && !in_comment) {
      if (c == '{') {
        formatted += c;
        formatted += '\n';
        indent += opts.indent_size;
        for (int j = 0; j < indent; ++j) formatted += ' ';
        continue;
      } else if (c == '}') {
        formatted += '\n';
        indent -= opts.indent_size;
        for (int j = 0; j < indent; ++j) formatted += ' ';
        formatted += c;
        continue;
      } else if (c == ';') {
        formatted += c;
        formatted += '\n';
        for (int j = 0; j < indent; ++j) formatted += ' ';
        continue;
      } else if (c == '\n') {
        if (opts.remove_trailing_whitespace) {
          // Remove trailing spaces
          while (!formatted.empty() && formatted.back() == ' ') {
            formatted.pop_back();
          }
        }
        formatted += c;
        for (int j = 0; j < indent; ++j) formatted += ' ';
        continue;
      }
    }

    formatted += c;
  }

  // Sort imports if enabled
  if (opts.sort_imports) {
    // Implement import sorting
  }

  return formatted;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: caps_formatter <file.caps> [--indent=2] [--align] [--sort-imports]\n";
    return 1;
  }

  FormatOptions opts;
  std::string file = argv[1];

  // Parse options
  for (int i = 2; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--align") opts.align_assignments = true;
    if (arg == "--sort-imports") opts.sort_imports = true;
    if (arg.substr(0, 9) == "--indent=") {
      opts.indent_size = std::stoi(arg.substr(9));
    }
  }

  std::ifstream ifs(file);
  if (!ifs) {
    std::cerr << "Cannot open file\n";
    return 1;
  }

  std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

  std::string formatted = format_code(content, opts);

  std::cout << formatted;
  return 0;
}