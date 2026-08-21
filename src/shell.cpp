#include "shell.h"
#include "command_utils.h"
// #include "commands.h"
#include "utils.h"
#include <filesystem>
#include <iostream>
#include <unordered_map>

namespace fs = std::filesystem;

Shell::Shell() {
  // build built-in commands

  current_path = fs::current_path();

  commands.insert({"exit", CommandBuilder::build_exit(*this)});
  commands.insert({"echo", CommandBuilder::build_echo(*this)});
  commands.insert({"type", CommandBuilder::build_type(*this)});
  commands.insert({"pwd", CommandBuilder::build_pwd(*this)});
  commands.insert({"cd", CommandBuilder::build_cd(*this)});
}

void Shell::exit() { m_exit_flag = true; }
void Shell::printLine(std::string_view s) { std::cout << s << "\n"; }

void Shell::writeToFiles(const std::string &what,
                         const std::vector<std::string> &paths) {
  for (auto path : paths) {
    writeToFile(path, what);
  }
}

void Shell::writeToOutputFiles(const std::string &what) {
  writeToFiles(what, command_args.output_files);
}
void Shell::output(const std::string &s) {
  if (command_args.output_files.size() > 0) {
    // output to file
    writeToOutputFiles(s);

  } else {
    printLine(s);
  }
}

void Shell::writeErrorToFiles(const std::string &what) {
  writeToFiles(what, command_args.error_output_files);
}

void Shell::outputError(const std::string &what) {
  if (command_args.error_output_files.size() > 0) {
    writeErrorToFiles(what);
  } else {
    printLine(what);
  }
}

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

    command_args = parse_command(com);
    // std::cout << command_args.args[0];

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
