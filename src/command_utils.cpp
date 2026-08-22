#include "command_utils.h"
#include "parser.h"
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

CommandArgs parse_command(const std::string &s) {
  std::vector<std::string> tokens = parseCommand(s);
  int cursor = 0;

  TokenGrammarParser parser(tokens);
  parser.parseSentence();

  return parser.getCommandArgs();
}

std::string is_executable(const std::string &com) {
  // Important: ':' is only delimiter in linux. in windows its ';'.
  std::vector<std::string> paths = split_string(getenv("PATH"), ':');
  for (const auto &path : paths) {
    if (!fs::exists(path)) {
      continue;
    }
    for (const auto &entry : fs::directory_iterator(path)) {
      // check for exec permissions using fs.
      bool has_exec = (fs::status(entry).permissions() &
                       fs::perms::owner_exec) != fs::perms::none;
      if (entry.path().stem().string() == com && has_exec) {
        return entry.path().string();
      }
    }
  }

  return "NO";
}

std::unordered_set<std::string> get_all_executables() {
  std::unordered_set<std::string> executables;
  std::vector<std::string> paths = split_string(getenv("PATH"), ':');
  for (const auto &path : paths) {
    if (!fs::exists(path)) {
      continue;
    }
    for (const auto &entry : fs::directory_iterator(path)) {
      // check for exec permissions using fs.
      bool has_exec = (fs::status(entry).permissions() &
                       fs::perms::owner_exec) != fs::perms::none;
      if (has_exec) {
        executables.insert(entry.path().filename().string());
      }
    }
  }

  return executables;
}

void createFile(const std::string &path) {
  std::ofstream file;
  file.open(path, std::ofstream::out);
  file.close();
}

void writeToFile(const std::string &path, const std::string &what) {

  std::ofstream file;
  file.open(path, std::ofstream::out);

  file << what << "\n";
  file.close();
}

void appendToFile(const std::string &path, const std::string &what) {
  std::ofstream file;
  file.open(path, std::ofstream::app);
  file << what << "\n";
  file.close();
}
