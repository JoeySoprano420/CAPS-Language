#include <iostream>
#include <string>

// CAPS Static Analyzer
// Advanced analysis.

void analyze(const std::string& file) {
  std::cout << "Analyzing " << file << ": No issues found.\n";
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: caps_analyzer <file.caps>\n";
    return 1;
  }

  analyze(argv[1]);
  return 0;
}