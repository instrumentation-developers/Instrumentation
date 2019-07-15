#include "Command.h"

Command::Command(
  std::string command_name,
  std::string description,
  std::vector<Argument> arguments,
  void (*command_fnc)(std::vector<std::string>)) {

    this -> command_name = command_name;
    this -> description = description;

    this -> arguments = arguments;
    this -> command_fnc = command_fnc;
  }

void Command::execute(std::vector<std::string> arguments) {
  command_fnc(arguments);
}

std::string Command::get_name() {
  return this -> command_name;
}

std::string Command::get_description() {
  return this -> description;
}

std::vector<Argument> Command::get_arguments() {
  return this -> arguments;
}

std::string Command::get_help() {
  std::ostringstream help_str;

  help_str.setf(std::ios_base::left);

  help_str.width(10);
  help_str << command_name;

  help_str.width(4);
  help_str << " ";

  help_str.width(100);
  help_str << description << std::endl;

  for (int i = 0; i < arguments.size(); i++) {

    help_str << "   ";
    help_str.width(14);
    help_str << " ";

    std::string argument_str = "<" + arguments[i].argument_name + ">";
    help_str.width(20);
    help_str << argument_str;

    help_str.width(4);
    help_str << " ";

    help_str << "-" << arguments[i].description << std::endl;
  }

  help_str << std::endl;
  return help_str.str();
}
