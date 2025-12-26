#pragma once
#include <string>

namespace caps::aot {

// Writes `cpp_source` to disk and builds an .exe.
// Returns true on success.
bool compile_exe(const std::string& cpp_source,
                 const std::string& out_exe_path,
                 bool optO2 = true,
                 const std::string& work_dir = ".");

} // namespace caps::aot
