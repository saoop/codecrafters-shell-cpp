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

  ParseStates state = SKIP_SPACE_INITIAL;

  std::string out;

  while (current_index < n) {
    if (state == SKIP_SPACE_INITIAL) {
      if (s[current_index] != ' ') {
        if (s[current_index] == '\'') {
          state = NAME_QUOTES;
        } else if (s[current_index] == '"') {
          state = NAME_DOUBLE_QUOTES;
        } else {
          out.push_back(s[current_index]);
          state = NAME;
        }
      }
    }

    else if (state == NAME) {
      if (s[current_index] == '\'') {
        // enter the quotations
        state = NAME_QUOTES;
      } else if (s[current_index] == '"') {
        // enter double quotations
        state = NAME_DOUBLE_QUOTES;
      } else if (s[current_index] == ' ') {
        // fully parsed
        state = SKIP_SPACE_ARG;
        command_args.command_name = std::move(out);
        out.clear();
      } else {
        out.push_back(s[current_index]);
      }
    }

    else if (state == NAME_QUOTES) {
      if (s[current_index] == '\'') {
        // exit quotations
        state = NAME;
      } else {
        out.push_back(s[current_index]);
      }
    }

    else if (state == NAME_DOUBLE_QUOTES) {
      if (s[current_index] == '"') {
        // exit quotations
        state = NAME;
      } else {
        out.push_back(s[current_index]);
      }
    }

    else if (state == SKIP_SPACE_ARG) {
      if (s[current_index] != ' ') {
        if (s[current_index] == '\'') {
          // we go to quotes
          state = ARGS_QUOTES;
        } else if (s[current_index] == '"') {
          // we go double quotes
          state = ARGS_DOUBLE_QUOTES;
        } else {
          // we go to letter parsing
          state = ARGS;
          out.push_back(s[current_index]);
        }
      }
    }

    else if (state == ARGS) {
      if (s[current_index] == '\'') {
        // we go to quotes
        state = ARGS_QUOTES;
      } else if (s[current_index] == '"') {
        state = ARGS_DOUBLE_QUOTES;
      } else if (s[current_index] == ' ') {
        // Parsed the full argument, go to skip spaces
        command_args.args.push_back(std::move(out));
        out.clear();
        state = SKIP_SPACE_ARG;
      } else {
        // we parse letter
        out.push_back(s[current_index]);
      }
    }

    else if (state == ARGS_QUOTES) {
      if (s[current_index] == '\'') {
        // we go back to letters
        state = ARGS;
      } else {
        // we parse letter or space inside of the quotes
        out.push_back(s[current_index]);
      }
    }

    else if (state == ARGS_DOUBLE_QUOTES) {
      if (s[current_index] == '"') {
        // exit to letters
        state = ARGS;
      }

      else {
        out.push_back(s[current_index]);
      }
    }

    current_index++;
  }

  if (state == NAME) {
    command_args.command_name = std::move(out);
  }
  if (state == ARGS) {
    command_args.args.push_back(std::move(out));
  }

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
