#include "command_utils.h"

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
Command CommandBuilder::build_pwd(Shell &shell) {
  Command pwd_command{
      .name = "pwd",
      .handler = [&shell](std::vector<std::string> args,
                          std::unordered_map<std::string, std::string> kargs) {
        shell.print_line(shell.get_current_path().string());
      }};

  return pwd_command;
};

Command CommandBuilder::build_cd(Shell &shell) {
  Command cd_command{
      .name = "cd",
      .handler = [&shell](std::vector<std::string> args,
                          std::unordered_map<std::string, std::string> kargs) {
        if (args.size() == 0 || args[0].size() == 0) {
          shell.print_line("Specify the directory");
        }

        std ::string to_path = "";

        if (args[0][0] == '~') {
          // change  to  home dir as root.
          std::string home_dir = getenv("HOME");
          to_path += home_dir;
          to_path += args[0].substr(1, args[0].size() - 1);

        } else {
          to_path += args[0];
        }

        fs::path p = to_path;

        if (p.is_relative()) {
          if (!fs::exists(shell.get_current_path() / p)) {
            shell.print_line(args[0] + ": No such file or directory");
            return;
          }
          p = fs::canonical(shell.get_current_path() / p);
        }

        // Now we sure we have absolute path
        if (!fs::exists(p)) {
          shell.print_line(args[0] + ": No such file or directory");
          return;
        }
        if (!fs::is_directory(p)) {
          shell.print_line(args[0] + "is not a directory");
          return;
        }
        shell.set_current_path(p);
      }};
  return cd_command;
}
