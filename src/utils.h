#pragma once
#include <string>
#include <vector>

std::vector<std::string> split_string(const std::string &s, char delim);

bool writeToFile(const std::string &s);