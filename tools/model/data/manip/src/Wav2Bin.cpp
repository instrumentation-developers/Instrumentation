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
std::vector<WaveFile*> audio_files;

void list_fnc(std::vector<std::string> arguments);
void help_fnc(std::vector<std::string> arguments);
void load_fnc(std::vector<std::string> arguments);
void binary_fnc(std::vector<std::string> arguments);
void split_fnc(std::vector<std::string> arguments);
void resample_fnc(std::vector<std::string> arguments);
void mono_fnc(std::vector<std::string> arguments);
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
  "Loads all audio files from the specified directory.",
  std::vector<Argument>{
    (Argument){"file-directory", "the directory from which to load"}
  },
  &load_fnc
);
Command binary(
  "binary",
  "Converts all loaded audio files to binary files.",
  std::vector<Argument>{
    (Argument){"file-directory", "the directory to which to save the file(s)"},
    (Argument){"name", "the name under which to save the file(s)"}
  },
  &binary_fnc
);
Command split(
  "split",
  "Splits all loaded audio files into clips of the specified duration.",
  std::vector<Argument>{
    (Argument){"length", "the length of each clip"}
  },
  &split_fnc
);
Command resample(
  "resample",
  "Resamples all loaded audio files at the specified frequency.",
  std::vector<Argument>{
    (Argument){"frequency", "the frequency at which to sample (in Hz)"}
  },
  &resample_fnc
);
Command mono(
  "mono",
  "Averages all channels across each loaded audio file.",
  std::vector<Argument>{},
  &mono_fnc
);
Command exit_cmd(
  "exit",
  "Exits the program.",
  std::vector<Argument>{},
  &exit_fnc
);

std::vector<Command> commands{list, help, load, binary, split, resample, mono, exit_cmd};

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
    WaveFile* audio_file = new WaveFile();

    audio_file -> read(dir_path + "\\" + files[i]);
    audio_files.push_back(audio_file);
  }
  std::cout << ">> Loaded " << files.size() << " file(s).\n";
}

void split_fnc(std::vector<std::string> arguments) {

  std::cout << ">> Splitting all files.\n";

  std::vector<WaveFile*> temp_files;
  for (int i = 0; i < audio_files.size(); i++) {
    temp_files.push_back(audio_files[i]);
  }

  audio_files.clear();

  for (int i = 0; i < temp_files.size(); i++) {
    std::vector<WaveFile*> clips;
    temp_files[i] -> split(std::stoi(arguments[0]), clips);

    for (int j = 0; j < clips.size(); j++) {
      audio_files.push_back(clips[j]);
    }
  }

  std::cout << ">> Split " << temp_files.size() << " file(s) into " << audio_files.size() << " files(s).\n";
}

void binary_fnc(std::vector<std::string> arguments) {
  std::cout << ">> Extracting binary files.\n";

  std::string dir_path = arguments[0];
  std::string name = arguments[1];

  int digits = (int)std::floor(std::log10(audio_files.size())) + 1;

  for (int i = 0; i < audio_files.size(); i++) {
    BinaryFile binary_file(audio_files[i] -> get_combined_data());

    std::stringstream ss;
    ss << std::setw(digits) << std::setfill('0') << i;
    std::string s = ss.str();

    binary_file.write(dir_path + "\\" + name + s + ".bin");
  }

  std::cout << ">> Extracted " << audio_files.size() << " file(s).\n";
}

void resample_fnc(std::vector<std::string> arguments) {

  int sample_rate = 0;

  try {
    sample_rate = stoi(arguments[0]);
  }
  catch (const std::invalid_argument &ia) {
    std::cout << ">> ERROR: <sample_rate> must be an integer.\n";
    return;
  }

  std::cout << ">> Resampling at " << sample_rate << " Hz.\n";

  for (int i = 0; i < audio_files.size(); i++) {
    audio_files[i] -> resample(sample_rate);
  }

  std::cout << ">> Resampling complete.\n";
}

void mono_fnc(std::vector<std::string> arguments) {
    std::cout << ">> Converting to mono.\n";

    for (int i = 0; i < audio_files.size(); i++) {
      audio_files[i] -> to_mono();
    }

    std::cout << ">> Conversion complete.\n";
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
