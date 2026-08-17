#include <iostream>
#include <string>

bool checkValidCommand(std::string &com) { return false; }

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::cout << "$ ";

  std::string com;

  while (true) {
    std::getline(std::cin, com);
    if (checkValidCommand(com)) {
      if (com == "exit") {
        return 0;
      }
    } else {
      std::cout << com << ": command not found\n";
      std::cout << "$ ";
    }
  }
}
