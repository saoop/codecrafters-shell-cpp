#include "commands.h"
#include "command_utils.h"
#include <string>
#include <unordered_map>
#include <vector>
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
        if (args.size() == 0) {
          shell.output("");
          return;
        }

        for (int i = 0; i < args.size() - 1; i++) {
          full_str += args[i];
          full_str += " ";
        }

        full_str += args[args.size() - 1];
        shell.output(full_str);
      }};

  return echo_command;
}

Command CommandBuilder::build_type(Shell &shell) {

  Command type_command{
      .name = "type",
      .handler = [&shell](std::vector<std::string> args,
                          std::unordered_map<std::string, std::string> kargs) {
        if (args.size() == 0) {
          shell.outputError("Provide an argument");
          return;
        }

        if (shell.is_built_in(args[0])) {
          shell.output(args[0] + " is a shell builtin");

        } else {

          std::string path = is_executable(args[0]);
          if (path == "NO") {
            shell.outputError(args[0] + ": not found");
          } else {
            shell.output(args[0] + " is " + path);
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
        shell.output(shell.get_current_path().string());
      }};

  return pwd_command;
};

Command CommandBuilder::build_cd(Shell &shell) {
  Command cd_command{
      .name = "cd",
      .handler = [&shell](std::vector<std::string> args,
                          std::unordered_map<std::string, std::string> kargs) {
        if (args.size() == 0 || args[0].size() == 0) {
          shell.outputError("Specify the directory");
          return;
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
            shell.outputError(args[0] + ": No such file or directory");
            return;
          }
          p = fs::canonical(shell.get_current_path() / p);
        }

        // Now we sure we have absolute path
        if (!fs::exists(p)) {
          shell.outputError(args[0] + ": No such file or directory");
          return;
        }
        if (!fs::is_directory(p)) {
          shell.outputError(args[0] + "is not a directory");
          return;
        }
        shell.set_current_path(p);
      }};
  return cd_command;
}

Command CommandBuilder::build_complete(Shell &shell) {
  Command complete_command{
      .name = "complete",
      .handler = [&shell](std::vector<std::string> args,
                          std::unordered_map<std::string, std::string> flags) {
        // just pass all args and handle it myself.

        struct Args {
          bool p_flag = false;
          std::string p_command;
          bool C_flag = false;
          std::pair<std::string, std::string> C_path_comm;
        };

        Args parsed_args;
        int cursor{};
        while (cursor < args.size()) {
          // std::cout << args[cursor];
          if (args[cursor] == "-p") {
            parsed_args.p_flag = true;
            parsed_args.p_command = args[++cursor];
          } else if (args[cursor] == "-C") {
            parsed_args.C_flag = true;
            parsed_args.C_path_comm = {args[++cursor], args[++cursor]};
          }
          cursor++;
        }

        if (parsed_args.p_flag) {
          if (shell.hasCompletion(parsed_args.p_command)) {
            shell.output("complete -C " +
                         shell.getCompletionPath(parsed_args.p_command) + " " +
                         parsed_args.p_command);
          } else {
            shell.outputError("complete: " + parsed_args.p_command +
                              ": no completion specification");
          }
        }

        if (parsed_args.C_flag) {
          shell.setCompletion(parsed_args.C_path_comm.second,
                              parsed_args.C_path_comm.first);
        }
      }};

  return complete_command;
}