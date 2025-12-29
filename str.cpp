#include "util/str.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::string read_file_to_string(const std::string& path) {
  std::ifstream in(path, std::ios::binary);
  if (!in) throw std::runtime_error("failed to open file: " + path);
  in.seekg(0, std::ios::end);
  size_t size = in.tellg();
  const size_t MAX_FILE_SIZE = 10 * 1024 * 1024; // 10MB limit
  if (size > MAX_FILE_SIZE) throw std::runtime_error("file too large: " + path);
  in.seekg(0, std::ios::beg);
  std::string content(size, '\0');
  in.read(&content[0], size);
  if (!in) throw std::runtime_error("failed to read file: " + path);
  return content;
}
