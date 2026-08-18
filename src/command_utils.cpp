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
