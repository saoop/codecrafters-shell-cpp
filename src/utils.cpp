#include "utils.h"
#include "parser.h"
#include <memory>
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

bool checkStrOnlySpaces(const std::string &s) {
  for (auto &c : s) {
    if (c != '\n' && c != '\t' && c != ' ') {
      return false;
    }
  }
  return true;
}

bool hadCommandName(std::vector<std::string> &tokens) {
  int i{1};
  while (i < tokens.size()) {
    if (isFileCharacter(tokens[i])) {
      i += 2; // skip current and next, since next must be a filename
    } else {
      return true;
    }
  }
  //  skipped all tokens -> only filenames and '>'-like tokens
  return false;
}

std::string findCommandName(std::vector<std::string> &tokens) {
  int i{};
  while (i < tokens.size()) {
    if (isFileCharacter(tokens[i])) {
      // skip
      i += 2;
    } else {
      return tokens[i];
    }
  }

  return "";
}

std::vector<std::string> findMatchingFiles(fs::path pwd, fs::path &sub_path,
                                           std::string &search_token) {
  std::vector<std::string> matches = {};
  for (auto &dir : fs::directory_iterator(pwd / sub_path)) {
    std::string candidate = dir.path().filename().string();
    if (candidate.starts_with(search_token)) {
      std::string match = (sub_path / candidate).string();
      if (fs::is_directory(pwd / match)) {
        matches.push_back(match + "/");
      } else {
        matches.push_back(match);
      }
    }
  }

  return matches;
}

void TrieCompletions::traverse(std::vector<std::string> &out,
                               TrieNode *currentNode,
                               std::string currentString) {
  if (currentNode->is_word) {
    out.push_back(currentString);
  }

  // If it's a word -> children can be empty. But we need all possible
  // completions.
  for (const auto &[key, node_ptr] : currentNode->children) {
    traverse(out, node_ptr.get(), currentString + key);
  }
}

std::vector<std::string> TrieCompletions::completions(const std::string &what) {
  std::vector<std::string> out;
  TrieNode *node = &root;
  for (char c : what) {
    if (!node->has(c)) {
      return out; // no such command found
    }
    node = node->children[c].get();
  }

  // if 'what' is incomplete, traverse the trie to find all matches
  traverse(out, node, what);

  return out;
}

bool TrieCompletions::search(const std::string &what) {
  // Searches the prefix tree for word 'what'
  TrieNode *node = &root;
  for (char c : what) {
    if (!node->has(c)) {
      return false;
    }
    node = node->children[c].get();
  }
  if (node->is_word) {
    return true;
  }
  return false;
}
void TrieCompletions::insert(const std::string what) {
  TrieNode *node = &root;
  for (char c : what) {
    if (node->has(c)) {
      // continue;
    } else {
      node->children[c] = std::make_unique<TrieNode>();
    }
    // need get() because it gets the raw address of unique_ptr
    node = node->children[c].get();
  }
  node->is_word = true;
}
