#include "shell.h"
#include "command_utils.h"
// #include "commands.h"
#include "utils.h"
#include <iostream>
#include <unordered_map>

#include <filesystem>

namespace fs = std::filesystem;

Shell::Shell() {
  // build built-in commands

  current_path = fs::current_path();

  commands.insert({"exit", CommandBuilder::build_exit(*this)});
  commands.insert({"echo", CommandBuilder::build_echo(*this)});
  commands.insert({"type", CommandBuilder::build_type(*this)});
  commands.insert({"pwd", CommandBuilder::build_pwd(*this)});
}

void Shell::exit() { m_exit_flag = true; }
void Shell::print_line(std::string_view s) { std::cout << s << "\n"; }

bool Shell::is_built_in(const std::string &s) const {
  return commands.count(s) > 0;
}

std::string Shell::execute(const std::string &com) {
  std::array<char, 128> buffer;
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(com.c_str(), "r"),
                                                pclose);
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) !=
         nullptr) {
    result += buffer.data();
  }
  return result;
};

void Shell::start() {
  while (!m_exit_flag) {
    std::string com;
    std::cout << "$ ";
    std::getline(std::cin, com);

    CommandArgs command_args = get_command_args(split_string(com, ' '));

    // check if it's a built in command
    if (commands.count(command_args.command_name)) {
      commands[command_args.command_name].handler(command_args.args, {});
      continue;
    }
    // check if it's executable
    std::string path = is_executable(command_args.command_name);
    if (path != "NO") {
      std::string result = execute(com);
      std::cout << result;
    }

    else {
      std::cout << com << ": command not found\n";
    }
  }
}
