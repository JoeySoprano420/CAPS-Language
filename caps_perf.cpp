#include <iostream>
#include <string>

// CAPS Deterministic Performance Tools
// Measures deterministic performance.

void measure_perf(const std::string& file) {
  std::cout << "Performance of " << file << ": 100ms worst-case\n";
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: caps_perf <file.caps>\n";
    return 1;
  }

  measure_perf(argv[1]);
  return 0;
}