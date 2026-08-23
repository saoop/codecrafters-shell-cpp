#include "shell.h"

int main() {
  Shell &shell = Shell::getInstance();
  shell.start();
}
