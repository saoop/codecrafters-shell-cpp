// shell.h
#pragma once
#include "commands.h"
#include <string>
#include <string_view>
#include <unordered_map>

class Shell {
public:
  Shell();
  void exit();
  void print_line(std::string_view s);
  bool is_built_in(const std::string &s) const;
  void start();
  std::string execute(const std::string &com);

  // Disable copies
  Shell(const Shell &) = delete;
  Shell &operator=(const Shell &) = delete;

private:
  bool m_exit_flag{false};
  std::unordered_map<std::string, Command> commands;
};
