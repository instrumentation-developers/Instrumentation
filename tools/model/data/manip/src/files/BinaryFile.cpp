#include <iostream>
#include <cstring>
#include <bitset>

#include "BinaryFile.h"

BinaryFile::BinaryFile() {
  this -> data = std::vector<char>{};
}

BinaryFile::BinaryFile(std::vector<char> data) {
  this -> data = data;
}

void BinaryFile::read(std::string file_name) {
  FILE *file_ptr = fopen(file_name.c_str(), "rb");

  if (file_ptr == NULL) {
    std::cout << "ERROR: Unable to open file \"" << file_name << "\".\n";
    return;
  }

  char data_buffer[1];

  while(!feof(file_ptr)) {
      fread(data_buffer, sizeof(data_buffer), 1, file_ptr);

      int sample_data;
      btoi(data_buffer, 1, sample_data);
      data.push_back(sample_data);
  }

  fclose(file_ptr);
}

void BinaryFile::write(std::string file_name) {
  FILE *file_ptr = fopen(file_name.c_str(), "wb");

  if (file_ptr == NULL) {
    std::cout << "ERROR: Unable to open file \"" << file_name << "\".\n";
    return;
  }

  char data_buffer[1];

  for (long i = 0; i < data.size(); i++) {
      int sample_data = data[i];

      itob(sample_data, data_buffer, 1);
      fwrite(data_buffer, sizeof(data_buffer), 1, file_ptr);
  }
  fclose(file_ptr);
}

/*accessors*/
std::vector<char> BinaryFile::get_data() {
  return this -> data;
}
