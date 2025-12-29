#include <iostream>
#include <string>
#include <fstream>

// CAPS Documentation Generator
// Generates docs from source.

void generate_docs(const std::string& input, const std::string& output) {
  std::ifstream ifs(input);
  std::ofstream ofs(output + "/index.html");
  ofs << "<html><body><h1>CAPS Docs</h1></body></html>\n";
  std::cout << "Generated docs in " << output << "\n";
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "Usage: caps_docgen <input> --output=<dir>\n";
    return 1;
  }

  std::string input = argv[1];
  std::string output = argv[2] + 9;  // Skip --output=
  generate_docs(input, output);
  return 0;
}