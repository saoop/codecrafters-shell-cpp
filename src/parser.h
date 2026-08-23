#pragma once
#include "command_utils.h"

enum ParseStates {

  LETTERS,
  QUOTES,
  SKIP_SPACE,
  DOUBLE_QUOTES,
  LETTERS_BACKSLASH,
  QUOTES_BACKSLASH,
  DOUBLE_QUOTES_BACKSLASH

};

bool inline isSpecialCharacter(const std::string &s) {
  return s == ">" || s == "1>" || s == "2>" || s == ">>" || s == "1>>" ||
         s == "2>>";
}

std::vector<std::string> tokenizeCommand(const std::string &s);

bool is_proper_string(const std::string &s);

class TokenGrammarParser {
private:
  int m_cursor;
  std::vector<std::string> m_tokens;
  CommandArgs m_CommandArgs;

public:
  TokenGrammarParser(const std::vector<std::string> &s) {
    m_cursor = 0;
    m_tokens = s;
  }
  bool parseSentence();
  bool parseName();
  bool parseOutputBegin();
  bool parseOutputArgs();
  bool parseOutputArg();
  CommandArgs getCommandArgs();
};
