#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
struct Command {
  std::string name;
  std::function<void(std::vector<std::string> args,
                     std::unordered_map<std::string, std::string> kargs)>
      handler;
}; // namespace fsstruct Command

struct CommandArgs {
  std::string command_name;
  std::vector<std::string> args;
  std::unordered_map<std::string, std::string> kargs;
};