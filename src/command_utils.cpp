#include "command_utils.h"
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

// will need a proper parser later?

CommandArgs get_command_args(const std::vector<std::string> &s) {
  CommandArgs command_args;

  command_args.command_name = s[0];
  command_args.args = std::vector<std::string>(s.begin() + 1, s.end());

  return command_args;
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
