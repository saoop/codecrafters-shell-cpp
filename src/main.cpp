#include <iostream>
#include <string>

bool checkValidCommand(std::string &com) { return false; }

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // TODO: Uncomment the code below to pass the first stage
  std::cout << "$ ";

  std::string com;
  std::cin >> com;
  if (checkValidCommand(com)) {

  } else {
    std::cout << com << ": command not found";
  }
}
