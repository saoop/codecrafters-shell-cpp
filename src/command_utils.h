#pragma once
#include "commands.h"
#include "shell.h"
#include "utils.h"
#include <filesystem>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace fs = std::filesystem;

CommandArgs get_command_args(const std::vector<std::string> &s);

std::string is_executable(const std::string &com);

std::unordered_set<std::string> get_all_executables();

void createFile(const std::string &path);
void writeToFile(const std::string &path, const std::string &what);
void appendToFile(const std::string &path, const std::string &what);

CommandArgs parse_command(const std::string &s);

class CommandBuilder {
public:
  static Command build_exit(Shell &shell);
  static Command build_echo(Shell &shell);
  static Command build_type(Shell &shell);
  static Command build_pwd(Shell &shell);
  static Command build_cd(Shell &shell);
  static Command build_complete(Shell &shell);
};
