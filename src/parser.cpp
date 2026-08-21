#include "parser.h"
#include <iostream>
#include <string>
#include <vector>

bool is_proper_string(const std::string &s) {
  if (s == ">")
    return false;
  return true;
}

std::vector<std::string> parseCommand(const std::string &s) {
  int next_index = 1;
  int last_index = 0;
  int current_index = 0;
  int n = s.size();

  ParseStates state = SKIP_SPACE;

  std::string out;

  std::vector<std::string> words;

  while (current_index < n) {

    if (state == SKIP_SPACE) {
      if (s[current_index] != ' ') {
        if (s[current_index] == '\'') {
          // we go to quotes
          state = QUOTES;
        } else if (s[current_index] == '"') {
          // we go double quotes
          state = DOUBLE_QUOTES;
        } else if (s[current_index] == '\\') {
          state = LETTERS_BACKSLASH;

        } else {
          // we go to letter parsing
          state = LETTERS;
          out.push_back(s[current_index]);
        }
      }
    }

    else if (state == LETTERS) {
      if (s[current_index] == '\'') {
        // we go to quotes
        state = QUOTES;
      } else if (s[current_index] == '"') {
        state = DOUBLE_QUOTES;
      } else if (s[current_index] == ' ') {
        // Parsed the full argument, go to skip spaces
        words.push_back(std::move(out));
        out.clear();
        state = SKIP_SPACE;
      } else if (s[current_index] == '\\') {
        // go to backlash
        state = LETTERS_BACKSLASH;
      } else {
        // we parse letter
        out.push_back(s[current_index]);
      }
    }

    else if (state == QUOTES) {
      if (s[current_index] == '\'') {
        // we go back to letters
        state = LETTERS;
      } else {
        // we parse letter or space inside of the quotes
        out.push_back(s[current_index]);
      }
    }

    else if (state == DOUBLE_QUOTES) {
      if (s[current_index] == '"') {
        // exit to letters
        state = LETTERS;
      } else if (s[current_index] == '\\') {
        // go to backlash
        state = DOUBLE_QUOTES_BACKSLASH;
      }

      else {
        out.push_back(s[current_index]);
      }
    }

    else if (state == LETTERS_BACKSLASH) {
      out.push_back(s[current_index]);
      state = LETTERS;
    } else if (state == DOUBLE_QUOTES_BACKSLASH) {
      // other wise its treated as a normal backslash
      char c = s[current_index];
      if (c == '\\' || c == '$' || c == '`' || c == '\n') {
        out.push_back(c);
        state = DOUBLE_QUOTES;

      } else if (c == '"' &&
                 (current_index == n - 1 || s[current_index + 1] == ' ')) {
        // if we close the quotes
        out.push_back('\\');
        state = DOUBLE_QUOTES;
      } else if (c == '"') {
        out.push_back(c);
        state = DOUBLE_QUOTES;

      } else {
        out.push_back('\\');
        out.push_back(c);
        state = DOUBLE_QUOTES;
      }
    }

    current_index++;
  }

  // parse the words array.
  // Also use a state machine.

  words.push_back(std::move(out));

  return words;
}

bool TokenGrammarParser::parseSentence() {
  // first parse the outputs

  // std::cout << "Parsing the sentence";

  if (!parseOutputBegin()) {
    return false;
  }

  // parse the name

  if (!parseName())
    return false;

  return parseOutputArgs();
}

bool TokenGrammarParser::parseOutputBegin() {
  if (m_cursor >= m_tokens.size()) {
    return true; // can be eps
  }

  // std::cout << "parsing OutputBegin\n";
  // std::cout << "current token: " << s[cursor] << '\n';

  // 1> filename

  //
  if (m_tokens[m_cursor] != ">" && m_tokens[m_cursor] != "1>") {
    return true; // can be eps
  }

  if (m_cursor >= m_tokens.size() - 1) {
    // return
    throw std::runtime_error("Parsing error: '>' must be followed by a string");
  } // another rule? string

  m_cursor++;

  if (m_tokens[m_cursor] == ">" || m_tokens[m_cursor] == "1>") {
    throw std::runtime_error("Parsing error: '>' must be followed by a string");
  }

  m_cursor++;

  return parseOutputBegin();
}

bool TokenGrammarParser::parseName() {
  if (m_cursor >= m_tokens.size()) {
    throw std::runtime_error(
        "Parsing error: name of the command not specified");
  }
  // std::cout << "parsing Name\n";
  // std::cout << "current token: " << s[cursor] << '\n';
  if (!is_proper_string(m_tokens[m_cursor])) {
    throw std::runtime_error(
        "Parsing error: name of the command must be a proper string");
  }
  m_CommandArgs.command_name = m_tokens[m_cursor];
  m_cursor++;

  return true;
}

bool TokenGrammarParser::parseOutputArg() {

  // std::cout << "parsing OutpuArg\n";
  // std::cout << "current token: " << s[cursor] << '\n';

  if ((m_tokens[m_cursor] == ">" || m_tokens[m_cursor] == "1>") &&
      m_cursor < m_tokens.size() - 1 &&
      is_proper_string(m_tokens[m_cursor + 1])) {
    m_cursor += 2;
    m_CommandArgs.output_files.push_back(m_tokens[m_cursor - 1]);
    return true;
  }

  else if (is_proper_string(m_tokens[m_cursor])) {
    m_CommandArgs.args.push_back(m_tokens[m_cursor]);
    m_cursor++;
    return true;
  }
  return false;
}

bool TokenGrammarParser::parseOutputArgs() {
  // either output file of the string or argument
  if (m_cursor >= m_tokens.size()) {
    return true; // can be eps
  }
  // std::cout << "parsing OutputArgs\n";
  // std::cout << "current token: " << s[cursor] << '\n';

  if (parseOutputArg()) {
    return parseOutputArgs();
  };
  return false;
}

CommandArgs TokenGrammarParser::getCommandArgs() { return m_CommandArgs; }