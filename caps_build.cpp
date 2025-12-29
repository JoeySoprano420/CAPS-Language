#include <iostream>
#include <string>
#include <filesystem>

// CAPS Build System
// Custom build tool.

namespace fs = std::filesystem;

void build_project(const std::string& dir) {
  fs::path build_dir = dir + "/build";
  if (!fs::exists(build_dir)) {
    fs::create_directory(build_dir);
  }

  // Simulate build
  std::cout << "Configuring with CMake...\n";
  std::cout << "Building...\n";
  std::cout << "Linking...\n";
  std::cout << "Build complete.\n";
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: caps_build <project_dir>\n";
    return 1;
  }

  build_project(argv[1]);
  return 0;
}