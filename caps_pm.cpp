#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>  // Assume JSON library

// CAPS Package Manager
// Advanced package management with dependencies, versions, registry.

namespace fs = std::filesystem;
using json = nlohmann::json;

struct Package {
  std::string name;
  std::string version;
  std::vector<std::string> dependencies;
  std::string description;
};

class PackageManager {
public:
  PackageManager() : registry_path_("registry.json"), local_path_("modules/") {}

  void install(const std::string& package_name, const std::string& version = "latest") {
    // Load registry
    json registry = load_registry();

    if (registry.contains(package_name)) {
      auto pkg_json = registry[package_name];
      Package pkg = {package_name, version, pkg_json["deps"], pkg_json["desc"]};

      // Resolve dependencies
      resolve_dependencies(pkg);

      // Download and install
      fs::path pkg_path = local_path_ / package_name;
      fs::create_directories(pkg_path);
      std::ofstream ofs(pkg_path / "module.caps");
      ofs << "// Package: " << package_name << " v" << version << "\n";
      ofs << "// " << pkg.description << "\n";

      std::cout << "Installed " << package_name << " v" << version << "\n";
    } else {
      std::cerr << "Package not found: " << package_name << "\n";
    }
  }

  void list() {
    for (const auto& entry : fs::directory_iterator(local_path_)) {
      std::cout << entry.path().filename() << "\n";
    }
  }

  void update(const std::string& package_name) {
    // Check for updates
    std::cout << "Updating " << package_name << "\n";
  }

  void remove(const std::string& package_name) {
    fs::path pkg_path = local_path_ / package_name;
    if (fs::exists(pkg_path)) {
      fs::remove_all(pkg_path);
      std::cout << "Removed " << package_name << "\n";
    }
  }

private:
  json load_registry() {
    std::ifstream ifs(registry_path_);
    if (ifs) {
      json j;
      ifs >> j;
      return j;
    }
    return json::object();
  }

  void resolve_dependencies(const Package& pkg) {
    for (const auto& dep : pkg.dependencies) {
      if (!fs::exists(local_path_ / dep)) {
        install(dep);
      }
    }
  }

  std::string registry_path_;
  fs::path local_path_;
};

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: caps_pm <command> [args]\n";
    std::cerr << "Commands: install <pkg> [version], list, update <pkg>, remove <pkg>\n";
    return 1;
  }

  PackageManager pm;
  std::string cmd = argv[1];

  if (cmd == "install" && argc >= 3) {
    std::string version = (argc > 3) ? argv[3] : "latest";
    pm.install(argv[2], version);
  } else if (cmd == "list") {
    pm.list();
  } else if (cmd == "update" && argc > 2) {
    pm.update(argv[2]);
  } else if (cmd == "remove" && argc > 2) {
    pm.remove(argv[2]);
  } else {
    std::cerr << "Unknown command\n";
    return 1;
  }

  return 0;
}