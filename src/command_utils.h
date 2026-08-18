#pragma once
#include "commands.h"
#include "shell.h"
#include "utils.h"
#include <filesystem>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

CommandArgs get_command_args(const std::vector<std::string> &s);

std::string is_executable(const std::string &com);

class CommandBuilder {
public:
  static Command build_exit(Shell &shell);
  static Command build_echo(Shell &shell);
  static Command build_type(Shell &shell);
};