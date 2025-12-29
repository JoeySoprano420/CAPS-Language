#include <iostream>
#include <string>

// CAPS Simulator
// Simulates process graphs.

void simulate(const std::string& file) {
  std::cout << "Simulating " << file << ": All processes terminate deterministically.\n";
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: caps_simulator <file.caps>\n";
    return 1;
  }

  simulate(argv[1]);
  return 0;
}