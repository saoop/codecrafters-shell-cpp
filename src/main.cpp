#include <iostream>
#include <string>
#include <unordered_map>

struct Command {
  std::string name;
};

bool checkValidCommand(std::unordered_map<std::string, Command> &commands,
                       std::string &com) {
  if (!commands.count(com)) {
    return false;
  }
  return true;
}

int main() {
  std::unordered_map<std::string, Command> commands;

  commands.insert({"exit", {"exit"}});

  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::cout << "$ ";

  std::string com;

  while (true) {
    std::getline(std::cin, com);
    if (commands.count(com)) {
      if (com == "exit") {
        return 0;
      }
    } else {
      std::cout << com << ": command not found\n";
      std::cout << "$ ";
    }
  }
}
