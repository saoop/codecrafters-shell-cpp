#include "command_utils.h"

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

Command CommandBuilder::build_exit(Shell &shell) {
  Command exit_command{
      .name = "exit",
      .handler = [&shell](std::vector<std::string> args,
                          std::unordered_map<std::string, std::string> kargs) {
        shell.exit();
      }};

  return exit_command;
}

Command CommandBuilder::build_echo(Shell &shell) {
  Command echo_command{
      .name = "echo",
      .handler = [&shell](std::vector<std::string> args,
                          std::unordered_map<std::string, std::string> kargs) {
        std::string full_str;
        for (int i = 0; i < args.size() - 1; i++) {
          full_str += args[i];
          full_str += " ";
        }

        full_str += args[args.size() - 1];
        shell.print_line(full_str);
      }};

  return echo_command;
}

Command CommandBuilder::build_type(Shell &shell) {

  Command type_command{
      .name = "type",
      .handler = [&shell](std::vector<std::string> args,
                          std::unordered_map<std::string, std::string> kargs) {
        if (args.size() == 0) {
          shell.print_line("Provide an argument");
          return;
        }

        if (shell.is_built_in(args[0])) {
          shell.print_line(args[0] + " is a shell builtin");

        } else {

          std::string path = is_executable(args[0]);
          if (path == "NO") {
            shell.print_line(args[0] + ": not found");
          } else {
            shell.print_line(args[0] + " is " + path);
          }
        }
      }};
  return type_command;
};
