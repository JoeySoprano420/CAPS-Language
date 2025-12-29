#include <iostream>
#include <string>
#include <chrono>

// CAPS Profiler Tool
// Profiles CAPS programs.

void profile_program(const std::string& file) {
  auto start = std::chrono::high_resolution_clock::now();
  // Simulate running the program
  std::cout << "Profiling " << file << "\n";
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "Execution time: " << duration.count() << "ms\n";
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: caps_profiler <file.caps>\n";
    return 1;
  }

  profile_program(argv[1]);
  return 0;
}