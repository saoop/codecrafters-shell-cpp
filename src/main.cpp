#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

struct Command {
  std::string name;
  std::function<void(std::vector<std::string> args,
                     std::unordered_map<std::string, std::string> kargs)>
      handler;
};

struct CommandArgs {
  std::string command_name;
  std::vector<std::string> args;
  std::unordered_map<std::string, std::string> kargs;
};
// will need a proper parser later?

CommandArgs split_string(const std::string &s, char delim) {
  size_t start = 0;
  size_t pos = 0;
  std::vector<std::string> out;
  CommandArgs command_args;

  int number_args{0};

  while (pos != std::string::npos) {
    pos = s.find(delim, start);

    // skip trailing spaces
    if (start == pos) {
      start++;
      continue;
    }

    // found 1st arg
    if (number_args == 0) {
      command_args.command_name = s.substr(start, pos - start);

    } else {
      command_args.args.push_back(s.substr(start, pos - start));
    }
    start = pos + 1;
    number_args++;
  }

  return command_args;
}

int main() {
  bool exit_flag = false;
  char delim = ' ';
  std::unordered_map<std::string, Command> commands;

  Command exit_command{
      .name = "exit",
      .handler =
          [&exit_flag](std::vector<std::string> args,
                       std::unordered_map<std::string, std::string> kargs) {
            exit_flag = true;
          }};

  Command echo_command{
      .name = "echo",
      .handler = [](std::vector<std::string> args,
                    std::unordered_map<std::string, std::string> kargs) {
        std::string full_str;
        for (int i = 0; i < args.size() - 1; i++) {
          full_str += args[i];
          full_str += " ";
        }

        full_str += args[args.size() - 1];

        std::cout << full_str << "\n";
      }};

  Command type_command{
      .name = "type",
      .handler =
          [&commands](std::vector<std::string> args,
                      std::unordered_map<std::string, std::string> kargs) {
            if (args.size() == 0) {
              std::cout << "Provide an argument\n";
              return;
            }

            if (commands.count(args[0])) {
              std::cout << args[0] << " is a shell bulletin\n";

            } else {
              std::cout << args[0] << ": not found\n";
            }
          }};

  commands.insert({"exit", exit_command});
  commands.insert({"echo", echo_command});
  commands.insert({"type", type_command});

  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::string com;

  while (!exit_flag) {
    std::cout << "$ ";

    std::getline(std::cin, com);

    // split?

    CommandArgs command_args = split_string(com, delim);

    if (commands.count(command_args.command_name)) {
      commands[command_args.command_name].handler(command_args.args, {});
    } else {
      std::cout << com << ": command not found\n";
    }
  }
}
