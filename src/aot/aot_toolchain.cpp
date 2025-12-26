#include "aot_toolchain.h"
#include <fstream>
#include <cstdlib>
#include <sstream>

#if defined(_WIN32)
  #include <windows.h>
#endif

namespace caps::aot {

static bool write_file(const std::string& path, const std::string& data) {
  std::ofstream f(path, std::ios::binary);
  if (!f) return false;
  f.write(data.data(), (std::streamsize)data.size());
  return true;
}

bool compile_exe(const std::string& cpp_source,
                 const std::string& out_exe_path,
                 bool optO2,
                 const std::string& work_dir) {
  const std::string cpp_path = work_dir + "/caps_generated.cpp";

  if (!write_file(cpp_path, cpp_source)) return false;

#if defined(_WIN32)
  // MSVC (cl.exe) expected available in PATH via "x64 Native Tools Command Prompt"
  std::ostringstream cmd;
  cmd << "cl /nologo /std:c++17 /EHsc ";
  if (optO2) cmd << "/O2 ";
  cmd << cpp_path << " /Fe:" << out_exe_path;

  int rc = std::system(cmd.str().c_str());
  return rc == 0;
#else
  // Prefer clang++, fallback g++
  std::ostringstream cmd;
  cmd << "sh -c \"";
  cmd << "if command -v clang++ >/dev/null 2>&1; then ";
  cmd << "clang++ -std=c++17 ";
  if (optO2) cmd << "-O2 ";
  cmd << cpp_path << " -o " << out_exe_path << "; ";
  cmd << "else g++ -std=c++17 ";
  if (optO2) cmd << "-O2 ";
  cmd << cpp_path << " -o " << out_exe_path << "; fi\"";

  int rc = std::system(cmd.str().c_str());
  return rc == 0;
#endif
}

} // namespace caps::aot
