#include "command_utils.h"
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
// will need a proper parser later?

std::vector<std::string> parse_string_command(const std::string &s) {
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

bool is_proper_string(const std::string &s) {
  if (s == ">")
    return false;
  return true;
}

// w -> OUTPUT_BEGIN NAME OUTPUT_ARGS
// OUTPUT_BEGIN -> >string | 1>string | OUTPUT_BEGIN | eps
// NAME -> string

// OUTPUT_ARGS -> OUTPUT ARG OUTPUT_ARGS | eps
// OUTPUT_ARG -> >string | 1>string | string| eps

using str_vec = const std::vector<std::string> &;

bool parseSentence(str_vec s, int &cursor, CommandArgs &command_args);
bool parseName(str_vec s, int &cursor, CommandArgs &command_args);
bool parseOutputBegin(str_vec s, int &cursor, CommandArgs &command_args);
bool parseOutputArgs(str_vec s, int &cursor, CommandArgs &command_args);
bool parseOutputArg(str_vec s, int &cursor, CommandArgs &command_args);
// bool parseOutput(str_vec s, int &cursor);
// bool parseArgs(str_vec s, int &cursor);

bool parseSentence(str_vec s, int &cursor, CommandArgs &command_args) {
  // first parse the outputs

  // std::cout << "Parsing the sentence";

  if (!parseOutputBegin(s, cursor, command_args)) {
    return false;
  }

  // parse the name

  if (!parseName(s, cursor, command_args))
    return false;

  return parseOutputArgs(s, cursor, command_args);
}

bool parseOutputBegin(str_vec s, int &cursor, CommandArgs &command_args) {
  if (cursor >= s.size()) {
    return true; // can be eps
  }

  // std::cout << "parsing OutputBegin\n";
  // std::cout << "current token: " << s[cursor] << '\n';

  // 1> filename

  //
  if (s[cursor] != ">" && s[cursor] != "1>") {
    return true; // can be eps
  }

  if (cursor >= s.size() - 1) {
    // return
    throw std::runtime_error("Parsing error: '>' must be followed by a string");
  } // another rule? string

  cursor++;

  if (s[cursor] == ">" || s[cursor] == "1>") {
    throw std::runtime_error("Parsing error: '>' must be followed by a string");
  }

  cursor++;

  return parseOutputBegin(s, cursor, command_args);
}

bool parseName(str_vec s, int &cursor, CommandArgs &command_args) {
  if (cursor >= s.size()) {
    throw std::runtime_error(
        "Parsing error: name of the command not specified");
  }
  // std::cout << "parsing Name\n";
  // std::cout << "current token: " << s[cursor] << '\n';
  if (!is_proper_string(s[cursor])) {
    throw std::runtime_error(
        "Parsing error: name of the command must be a proper string");
  }
  command_args.command_name = s[cursor];
  cursor++;

  return true;
}

bool parseOutputArg(str_vec s, int &cursor, CommandArgs &command_args) {

  // std::cout << "parsing OutpuArg\n";
  // std::cout << "current token: " << s[cursor] << '\n';

  if ((s[cursor] == ">" || s[cursor] == "1>") && cursor < s.size() - 1 &&
      is_proper_string(s[cursor + 1])) {
    cursor += 2;
    command_args.output_files.push_back(s[cursor - 1]);
    return true;
  }

  else if (is_proper_string(s[cursor])) {
    command_args.args.push_back(s[cursor]);
    cursor++;
    return true;
  }
  return false;
}

bool parseOutputArgs(str_vec s, int &cursor, CommandArgs &command_args) {
  // either output file of the string or argument
  if (cursor >= s.size()) {
    return true; // can be eps
  }
  // std::cout << "parsing OutputArgs\n";
  // std::cout << "current token: " << s[cursor] << '\n';

  if (parseOutputArg(s, cursor, command_args)) {
    return parseOutputArgs(s, cursor, command_args);
  };
  return false;
}

CommandArgs parse_command(const std::string &s) {
  std::vector<std::string> words = parse_string_command(s);
  int cursor = 0;

  CommandArgs command_args{};
  parseSentence(words, cursor, command_args);

  // command_args.command_name = std::move(words[0]);

  // command_args.args = std::vector<std::string>(words.begin() + 1,
  // words.end());

  return command_args;
}

std::string is_executable(const std::string &com) {
  // Important: ':' is only delimiter in linux. in windows its ';'.
  std::vector<std::string> paths = split_string(getenv("PATH"), ':');
  for (const auto &path : paths) {
    if (!fs::exists(path)) {
      continue;
    }
    for (const auto &entry : fs::directory_iterator(path)) {
      // check for exec permissions using fs.
      bool has_exec = (fs::status(entry).permissions() &
                       fs::perms::owner_exec) != fs::perms::none;
      if (entry.path().stem().string() == com && has_exec) {
        return entry.path().string();
      }
    }
  }

  return "NO";
}
bool writeToFile(const std::string &path, const std::string &what) {
  // if (fs::exists(path)) {
  //   throw std::runtime_error("Path does exist");
  // }
  // if()

  std::ofstream file;
  file.open(path, std::ofstream::out);

  file << what << "\n";
  file.close();

  return true;
}