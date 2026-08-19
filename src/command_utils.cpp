#include "command_utils.h"
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

// will need a proper parser later?

CommandArgs parse_command(const std::string &s) {
  int next_index = 1;
  int last_index = 0;
  int current_index = 0;
  int n = s.size();
  CommandArgs command_args{};

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
      out.push_back(s[current_index]);
      state = DOUBLE_QUOTES;
    }

    current_index++;
  }

  words.push_back(std::move(out));

  command_args.command_name = std::move(words[0]);
  command_args.args = std::vector<std::string>(words.begin() + 1, words.end());

  return command_args;
}

CommandArgs get_command_args(const std::vector<std::string> &s) {
  CommandArgs command_args;

  command_args.command_name = s[0];
  command_args.args = std::vector<std::string>(s.begin() + 1, s.end());

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
