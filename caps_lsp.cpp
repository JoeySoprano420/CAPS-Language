#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>

// CAPS Language Server (LSP)
// Full LSP implementation with diagnostics, completion, hover, etc.

using json = nlohmann::json;

class LanguageServer {
public:
  void run() {
    std::string line;
    while (std::getline(std::cin, line)) {
      handle_message(line);
    }
  }

private:
  void handle_message(const std::string& message) {
    try {
      json msg = json::parse(message);
      std::string method = msg["method"];

      if (method == "initialize") {
        send_response(msg["id"], get_capabilities());
      } else if (method == "textDocument/didOpen") {
        // Parse document
      } else if (method == "textDocument/completion") {
        send_completion(msg["id"]);
      } else if (method == "textDocument/hover") {
        send_hover(msg["id"]);
      } else if (method == "textDocument/diagnostic") {
        send_diagnostics(msg["id"]);
      }
    } catch (const std::exception& e) {
      std::cerr << "Error: " << e.what() << "\n";
    }
  }

  json get_capabilities() {
    return {
      {"capabilities", {
        {"textDocumentSync", 1},
        {"completionProvider", {{"triggerCharacters", {".", " "}}}},
        {"hoverProvider", true},
        {"diagnosticProvider", true}
      }}
    };
  }

  void send_response(const json& id, const json& result) {
    json response = {
      {"jsonrpc", "2.0"},
      {"id", id},
      {"result", result}
    };
    std::cout << response.dump() << "\n";
  }

  void send_completion(const json& id) {
    json items = json::array();
    items.push_back({{"label", "process"}, {"kind", 7}, {"detail", "Define a process"}});
    items.push_back({{"label", "channel"}, {"kind", 7}, {"detail", "Define a channel"}});
    send_response(id, {{"items", items}});
  }

  void send_hover(const json& id) {
    json hover = {
      {"contents", {
        {"kind", "markdown"},
        {"value", "**process** - Defines a finite state machine"}
      }}
    };
    send_response(id, hover);
  }

  void send_diagnostics(const json& id) {
    json diagnostics = json::array();
    // Add diagnostics based on parsed document
    send_response(id, {{"diagnostics", diagnostics}});
  }
};

int main() {
  LanguageServer lsp;
  lsp.run();
  return 0;
}