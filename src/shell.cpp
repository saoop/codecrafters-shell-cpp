#include "shell.h"
#include "command_utils.h"
#include "parser.h"
#include "utils.h"
#include <filesystem>
#include <iostream>
#include <memory>
#include <readline/history.h>
#include <readline/readline.h>
#include <unordered_map>

namespace fs = std::filesystem;

// Must be a global variable for readline lib to work
std::unique_ptr<TrieCompletions> completions =
    std::make_unique<TrieCompletions>();

bool hadCommandName(std::vector<std::string> &tokens) {
  int i{1};
  while (i < tokens.size()) {
    if (isSpecialCharacter(tokens[i])) {
      i += 2; // skip current and next, since next must be a filename
    } else {
      return true;
    }
  }
  //  skipped all tokens -> only filenames and '>'-like tokens
  return false;
}

char *completions_generator(const char *text, int state) {
  static std::vector<std::string> matches;

  static std::vector<std::string> file_matches;
  // since there are a lot of files

  // first parse.
  static size_t index;

  std::vector<std::string> tokens = tokenizeCommand(rl_line_buffer);

  bool had_command_name = hadCommandName(tokens);

  // check if the previous token indicates a filename
  bool is_prev_char_special =
      tokens.size() > 1 && isSpecialCharacter(tokens[tokens.size() - 2]);

  // empty char for the input of filename
  // bool command_written = tokens.size() == 1 &&
  // completions->search(tokens[0]);

  // when file:
  //  previous is >
  //  we had a string in the tokens.

  bool prev_command_then_empty = (tokens.size() > 0 && strcmp(text, "") == 0);

  // std::cout << (text == "") << "\n";

  if (is_prev_char_special || prev_command_then_empty || had_command_name) {

    // filename completion
    std::string incomplete_path = strcmp(text, "") == 0 ? "" : tokens.back();

    std::vector<std::string> split_path = split_string(incomplete_path, '/');
    std::string file_dir_to_search = "";

    if (!incomplete_path.ends_with('/')) {
      file_dir_to_search = split_path.back();
    }

    // if ending in '/' -> the last is '';

    // build the full path from pwd + split_path
    Shell &shell = Shell::getInstance();
    std::string path = "";

    for (int i{}; i < static_cast<int>(split_path.size()) - 1; i++) {
      path += (split_path[i] + "/");
    }
    // std::cout << path << "\n";

    if (state == 0) {
      matches = {};
      // first time call -> reconstruct filenameTrie
      // delete old one and assign a new pointer
      index = 0;
      // get all folders and files in path

      // // to avoid exceptions
      // if (!fs::is_directory(path)) {
      //   return {};
      // }

      fs::path path_to_search = shell.get_current_path() / path;
      fs::path starting = path;
      for (auto &dir : fs::directory_iterator(path_to_search)) {
        std::string candidate = dir.path().filename().string();
        if (candidate.starts_with(file_dir_to_search)) {
          std::string match = (starting / candidate).string();
          if (fs::is_directory(shell.get_current_path() / match)) {
            matches.push_back(match + "/");
          } else {
            matches.push_back(match);
          }
        }
      }
      if (matches.size() == 1) {
        rl_completion_append_character = '\0';
      }
    }
  } else {
    if (state == 0) {
      matches = completions->completions(text);
      index = 0;
    }
    // command name completion
  }

  if (index >= matches.size())
    return nullptr;

  return strdup(matches[index++].c_str());
}

Shell::Shell() {
  // build built-in commands

  // instance = this;

  current_path = fs::current_path();

  commands.insert({"exit", CommandBuilder::build_exit(*this)});
  commands.insert({"echo", CommandBuilder::build_echo(*this)});
  commands.insert({"type", CommandBuilder::build_type(*this)});
  commands.insert({"pwd", CommandBuilder::build_pwd(*this)});
  commands.insert({"cd", CommandBuilder::build_cd(*this)});

  completions = std::make_unique<TrieCompletions>();

  // add all built in commands into Trie
  for (auto &[name, _] : commands) {
    completions->insert(name);
  }

  for (auto &name : get_all_executables()) {
    completions->insert(name);
  }
}

void Shell::exit() { m_exit_flag = true; }
void Shell::printLine(std::string_view s) { std::cout << s << "\n"; }
void Shell::createFiles(const std::vector<std::string> &paths) {
  for (auto path : paths) {
    createFile(path);
  }
}
void Shell::writeToFiles(const std::string &what,
                         const std::vector<std::string> &paths) {
  for (auto path : paths) {
    writeToFile(path, what);
  }
}

void Shell::appendToFiles(const std::string &what,
                          const std::vector<std::string> &paths) {
  for (auto path : paths) {
    appendToFile(path, what);
  }
}

void Shell::output(const std::string &what) {
  if (command_args.output_files.size() > 0) {
    // output to file
    writeToFiles(what, command_args.output_files);
  }

  if (command_args.append_output_files.size() > 0) {
    appendToFiles(what, command_args.append_output_files);
  }

  if (command_args.append_output_files.size() == 0 &&
      command_args.output_files.size() == 0) {
    printLine(what);
  }
}

void Shell::outputError(const std::string &what) {
  if (command_args.error_files.size() > 0) {
    // output to file
    writeToFiles(what, command_args.error_files);
  }

  if (command_args.append_error_files.size() > 0) {
    appendToFiles(what, command_args.append_error_files);
  }

  if (command_args.append_error_files.size() == 0 &&
      command_args.error_files.size() == 0) {
    printLine(what);
  }
}

bool Shell::is_built_in(const std::string &s) const {
  return commands.count(s) > 0;
}

std::string Shell::execute(const std::string &com) {
  std::array<char, 128> buffer;
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(com.c_str(), "r"),
                                                pclose);
  if (!pipe) {
    throw std::runtime_error(
        "popen() failed!"); // TODO: use the output function.
  }
  while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) !=
         nullptr) {
    result += buffer.data();
  }
  return result;
};

void Shell::start() {

  while (!m_exit_flag) {
    std::string com;

    char *str;
    rl_attempted_completion_function = [](const char *text, int start,
                                          int end) -> char ** {
      // std::cout << text << " <- in complesitons\n";
      rl_attempted_completion_over = 1;
      rl_completion_append_character = ' ';
      return rl_completion_matches(text, completions_generator);
    };

    // Write an input handler.

    str = readline("$ ");
    com = strdup(str);
    free(str);

    if (checkStrOnlySpaces(com)) {
      continue;
    }

    command_args = parse_command(com);

    // Always create folders for errors and output.
    createFiles(command_args.output_files);
    createFiles(command_args.error_files);

    // check if it's a built in command
    if (commands.count(command_args.command_name)) {
      commands[command_args.command_name].handler(command_args.args, {});
      continue;
    }
    // check if it's executable
    std::string path = is_executable(command_args.command_name);
    if (path != "NO") { // make another command, like a ghost command with the
                        // same interface?
      std::string result = execute(com);
      std::cout << result;
    } else {
      std::cout << com << ": command not found\n";
    }
  }
}
