#include <iostream>
#include <dirent.h>
#include <string>
#include <sstream>
#include <vector>

#include "util/Utils.h"
#include "util/Command.h"
#include "files/WaveFile.h"
#include "files/BinaryFile.h"

bool running = true;
std::vector<BinaryFile*> binary_files;

void list_fnc(std::vector<std::string> arguments);
void help_fnc(std::vector<std::string> arguments);
void load_fnc(std::vector<std::string> arguments);
void audio_fnc(std::vector<std::string> arguments);
void exit_fnc(std::vector<std::string> arguments);

void not_found_fnc(std::string command) {
    std::cout << ">> Command \"" << command << "\" not found.\n";
}

Command list(
  "list",
  "Lists all commands.",
  std::vector<Argument>{},
  &list_fnc
);
Command help(
  "help",
  "Provides more information about each command.",
  std::vector<Argument>{(Argument){"command", "the command"}},
  &help_fnc
);
Command load(
  "load",
  "Loads all binary files from the specified directory.",
  std::vector<Argument>{
    (Argument){"file-directory", "the directory from which to load"},
  },
  &load_fnc
);
Command audio(
  "audio",
  "Converts all loaded binary files to audio files.",
  std::vector<Argument>{
    (Argument){"file-directory", "the directory to which to save"},
    (Argument){"name", "the name under which to save the file(s)"},
    (Argument){"audio-encoding", "the encoding format"},
    (Argument){"bytes-per-sample", "the number of bytes per sample"},
    (Argument){"sample-rate", "the sampling rate"},
    (Argument){"num-channels", "the number of channels"}
  },
  &audio_fnc
);
Command exit_cmd(
  "exit",
  "Exits the program.",
  std::vector<Argument>{},
  &exit_fnc
);

std::vector<Command> commands{list, help, load, audio, exit_cmd};

void list_fnc(std::vector<std::string> arguments) {
  std::cout << ">> ";
  for (int i = 0; i < commands.size() - 1; i++) {
    std::cout << commands[i].get_name() << ", ";
  }

  std::cout << commands[commands.size() - 1].get_name() << "\n";
}

void help_fnc(std::vector<std::string> arguments) {
  for (int i = 0; i < commands.size(); i++) {
    if (commands[i].get_name() == arguments[0]) {
      std::cout << ">> " << commands[i].get_help();
      return;
    }
  }

  not_found_fnc(arguments[0]);
}

void load_fnc(std::vector<std::string> arguments) {
  std::vector<std::string> files;

  std::string dir_path = arguments[0];

  int file_count = 0;

  std::cout << ">> Loading all files.\n";
  files = open_dir(dir_path);

  for (int i = 0; i < files.size(); i++) {
    BinaryFile* binary_file = new BinaryFile();

    binary_file -> read(dir_path + "\\" + files[i]);
    binary_files.push_back(binary_file);
  }
  std::cout << ">> Loaded " << files.size() << " file(s).\n";
}
void audio_fnc(std::vector<std::string> arguments) {
  std::cout << ">> Extracting files.\n";

  std::string dir_path = arguments[0];
  std::string name = arguments[1];

  unsigned int bytes_per_sample = (unsigned int) std::stoul(arguments[3]);
  WaveEncoding encoding_type = get_encoding_from_string(arguments[2]);

  unsigned int num_channels = (unsigned int) std::stoul(arguments[5]);
  unsigned int sample_rate = (unsigned int) std::stoi(arguments[4]);

  int digits = (int)std::floor(std::log10(binary_files.size())) + 1;

  for (int i = 0; i < binary_files.size(); i++) {
    std::stringstream ss;
    ss << std::setw(digits) << std::setfill('0') << i;
    std::string s = ss.str();

    WaveFile audio_file(binary_files[i] -> get_data(), num_channels, sample_rate, bytes_per_sample, encoding_type);
    audio_file.write(dir_path + "\\" + name + s + ".wav");
  }

  std::cout << ">> Extracted " << binary_files.size() << " file(s).\n";
}
void exit_fnc(std::vector<std::string> arguments) {
  running = false;
}


int main(int argc, char *argv[]) {

  std::cout << ">> Use \"list\" to view all commands.\n";

  while (running) {
    std::string command;
    std::vector<std::string> arguments;

    std::cout << "<< ";
    std::string input;
    getline(std::cin, input);

    std::istringstream iss(input);

    iss >> command;

    std::string argument;

    while(iss >> argument) {
        arguments.push_back(argument);
    }

    bool command_valid = false;
    bool arguments_valid = false;
    int command_id = 0;

    for (command_id = 0; command_id < commands.size(); command_id++) {
      if (command == commands[command_id].get_name()) {
        command_valid = true;
        if (arguments.size() == commands[command_id].get_arguments().size()) {
          arguments_valid = true;
        }
        break;
      }
    }

    if (command_valid) {
      if (arguments_valid) {
        commands[command_id].execute(arguments);
      } else {
        std::cout << ">> Invalid arguments. See below:\n";
        help.execute(std::vector<std::string>{command});
      }
    } else {
      not_found_fnc(command);
    }
  }

  return 0;
}
