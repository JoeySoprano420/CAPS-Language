#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <regex>
#include <unordered_set>

// CAPS Linter Tool
// Advanced linting with rules, severity levels, and fixes.

enum Severity { Info, Warning, Error };

struct LintRule {
  std::string name;
  Severity severity;
  std::regex pattern;
  std::string message;
  std::string fix_suggestion;
};

struct Issue {
  std::string rule;
  Severity severity;
  int line;
  std::string message;
  std::string suggestion;
};

class Linter {
public:
  Linter() {
    // Define rules
    rules_.push_back({"unused_var", Warning, std::regex(R"(var\s+(\w+)\s*=)"), "Unused variable", "Remove or use variable"});
    rules_.push_back({"long_line", Info, std::regex(""), "Line too long", "Break line"});
    rules_.push_back({"missing_annotation", Warning, std::regex(R"(group\s+\w+\s*\{)"), "Missing @pipeline_safe", "Add @pipeline_safe"});
    rules_.push_back({"inconsistent_spacing", Info, std::regex(R"(\w+\s*=\s*\w+)"), "Inconsistent spacing around =", "Use consistent spacing"});
  }

  std::vector<Issue> lint(const std::string& content) {
    std::vector<Issue> issues;
    std::istringstream iss(content);
    std::string line;
    int line_num = 1;
    std::unordered_set<std::string> defined_vars;

    while (std::getline(iss, line)) {
      for (const auto& rule : rules_) {
        std::smatch match;
        if (std::regex_search(line, match, rule.pattern)) {
          if (rule.name == "unused_var") {
            defined_vars.insert(match[1]);
          } else if (rule.name == "long_line" && line.size() > 80) {
            issues.push_back({rule.name, rule.severity, line_num, rule.message, rule.fix_suggestion});
          } else if (rule.name == "missing_annotation" && line.find("@pipeline_safe") == std::string::npos) {
            // Check if it's a group declaration
            if (line.find("group") != std::string::npos) {
              issues.push_back({rule.name, rule.severity, line_num, rule.message, rule.fix_suggestion});
            }
          } else if (rule.name == "inconsistent_spacing") {
            issues.push_back({rule.name, rule.severity, line_num, rule.message, rule.fix_suggestion});
          }
        }
      }
      ++line_num;
    }

    // Check for unused vars (simplified)
    for (const auto& var : defined_vars) {
      if (content.find(var + " ") == std::string::npos) {  // Rough check
        issues.push_back({"unused_var", Warning, 0, "Unused variable: " + var, "Remove variable"});
      }
    }

    return issues;
  }

private:
  std::vector<LintRule> rules_;
};

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: caps_linter <file.caps> [--fix] [--config=config.json]\n";
    return 1;
  }

  std::string file = argv[1];
  bool fix = false;
  std::string config;

  for (int i = 2; i < argc; ++i) {
    if (std::string(argv[i]) == "--fix") fix = true;
    if (std::string(argv[i]).substr(0, 9) == "--config=") config = argv[i] + 9;
  }

  std::ifstream ifs(file);
  if (!ifs) {
    std::cerr << "Cannot open file\n";
    return 1;
  }

  std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

  Linter linter;
  auto issues = linter.lint(content);

  for (const auto& issue : issues) {
    std::string sev_str = (issue.severity == Error) ? "Error" : (issue.severity == Warning) ? "Warning" : "Info";
    std::cout << sev_str << " [" << issue.rule << "]: " << issue.message << " at line " << issue.line << "\n";
    if (!issue.suggestion.empty()) {
      std::cout << "  Suggestion: " << issue.suggestion << "\n";
    }
  }

  if (fix) {
    // Implement auto-fix
    std::cout << "Auto-fix not implemented yet\n";
  }

  return issues.empty() ? 0 : 1;
}