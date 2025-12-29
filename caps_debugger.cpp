#include <iostream>
#include <string>
#include <map>

// CAPS Debugger Tool
// Simple debugger for CAPS.

std::map<std::string, bool> breakpoints;

void run_debugger() {
  std::string cmd;
  while (true) {
    std::cout << "(caps_debug) ";
    std::getline(std::cin, cmd);
    if (cmd == "quit") break;
    if (cmd.substr(0, 6) == "break ") {
      std::string bp = cmd.substr(6);
      breakpoints[bp] = true;
      std::cout << "Breakpoint set at " << bp << "\n";
    } else if (cmd == "run") {
      std::cout << "Running... (simulated)\n";
      for (const auto& bp : breakpoints) {
        if (bp.second) std::cout << "Hit breakpoint: " << bp.first << "\n";
      }
    } else {
      std::cout << "Unknown command\n";
    }
  }
}

int main(int argc, char* argv[]) {
  std::cout << "CAPS Debugger\n";
  run_debugger();
  return 0;
}