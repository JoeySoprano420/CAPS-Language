#include <iostream>
#include <string>

// Fuzzing harnesses

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  std::string input(reinterpret_cast<const char*>(data), size);
  // Fuzz parser
  try {
    // Parse input
  } catch (...) {
    // Ignore exceptions
  }
  return 0;
}