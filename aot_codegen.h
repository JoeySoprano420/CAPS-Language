#pragma once
#include "aot_ir_typed.h"
#include <string>

namespace caps::aot {

// Emits a single self-contained .cpp file.
// If emit_main=true, it includes a main() that runs group.schedule deterministically.
std::string emit_cpp(const Group& g, bool emit_main);

} // namespace caps::aot
