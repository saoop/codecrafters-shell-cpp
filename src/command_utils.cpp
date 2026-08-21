#include "command_utils.h"
#include "parser.h"
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
// will need a proper parser later?

// w -> OUTPUT_BEGIN NAME OUTPUT_ARGS
// OUTPUT_BEGIN -> >string | 1>string | OUTPUT_BEGIN | eps
// NAME -> string

// OUTPUT_ARGS -> OUTPUT ARG OUTPUT_ARGS | eps
// OUTPUT_ARG -> >string | 1>string | string| eps

// using str_vec = const std::vector<std::string> &;

// bool parseOutput(str_vec s, int &cursor);
// bool parseArgs(str_vec s, int &cursor);

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
bool writeToFile(const std::string &path, const std::string &what) {
  // if (fs::exists(path)) {
  //   throw std::runtime_error("Path does exist");
  // }
  // if()

  std::ofstream file;
  file.open(path, std::ofstream::out);

  file << what << "\n";
  file.close();

  return true;
}