#pragma once
#include "commands.h"
#include "utils.h"
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
namespace fs = std::filesystem;

class Shell {
public:
  Shell();

  void exit();
  void output(const std::string &s);
  void outputError(const std::string &s);
  bool is_built_in(const std::string &s) const;
  void start();
  void printLine(std::string_view s);
  void writeToFiles(const std::string &what,
                    const std::vector<std::string> &paths);
  void appendToFiles(const std::string &what,
                     const std::vector<std::string> &paths);
  void createFiles(const std::vector<std::string> &paths);
  std::string execute(const std::string &com);

  fs::path get_current_path() { return current_path; }

  void set_current_path(fs::path new_path) {
    if (fs::exists(new_path) && fs::is_directory(new_path)) {
      current_path = new_path;
    }
  }

  // Disable copies
  Shell(const Shell &) = delete;
  Shell &operator=(const Shell &) = delete;

private:
  // std::unique_ptr<TrieCompletions> completions;
  CommandArgs command_args;
  bool m_exit_flag{false};
  fs::path current_path;
  std::unordered_map<std::string, Command> commands;
};
