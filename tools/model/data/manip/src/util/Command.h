#ifndef COMMAND_H
#define COMMAND_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include <bitset>
#include <iomanip>

#include "../util/Utils.h"

typedef struct Argument {
  std::string argument_name;
  std::string description;
} Argument;

class Command {
public:
  //create a new Command object.
  Command(
    std::string command_name,
    std::string description,
    std::vector<Argument> arguments,
    void (*command_fnc)(std::vector<std::string>));

    void execute(std::vector<std::string> arguments);

    std::string get_name();
    std::string get_description();
    std::vector<Argument> get_arguments();

    std::string get_help();

private:
  std::string command_name;
  std::string description;

  std::vector<Argument> arguments;

  void (*command_fnc)(std::vector<std::string>);
};

#endif
