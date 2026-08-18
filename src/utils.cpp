#include "utils.h"
#include <string>
#include <vector>
std::vector<std::string> split_string(const std::string &s, char delim) {
  size_t start = 0;
  size_t pos = 0;
  std::vector<std::string> out;

  while (pos != std::string::npos) {
    pos = s.find(delim, start);

    // skip trailing spaces
    if (start == pos) {
      start++;
      continue;
    }

    out.push_back(s.substr(start, pos - start));
    start = pos + 1;
  }
  return out;
}