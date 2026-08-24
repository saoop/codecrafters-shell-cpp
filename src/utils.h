#pragma once
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;
std::vector<std::string> split_string(const std::string &s, char delim);

bool checkStrOnlySpaces(const std::string &s);

bool hadCommandName(std::vector<std::string> &tokens);

std::vector<std::string> findMatchingFiles(fs::path pwd, fs::path &sub_path,
                                           std::string &search_token);

std::string findCommandName(std::vector<std::string> &tokens);
struct TrieNode {
  // TrieNode class to  use in completions
  bool is_word = false;
  // unique ptr so that the TrieNode is recursively destroyed
  std::unordered_map<char, std::unique_ptr<TrieNode>> children;

  bool inline has(char c) { return children.count(c) > 0; }
  bool inline empty() { return children.size() == 0; }
};

class TrieCompletions {
private:
  TrieNode root;

  // currentString musnt be a reference, because we construct it.
  void traverse(std::vector<std::string> &out, TrieNode *currentNode,
                std::string currentString);

public:
  bool search(const std::string &what);
  void insert(const std::string what);
  std::vector<std::string> completions(const std::string &what);
};