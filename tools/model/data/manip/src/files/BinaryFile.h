#ifndef BINARYFILE_H
#define BINARYFILE_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include <string>
#include <vector>

#include "../util/Utils.h"
#include "../files/File.h"

class BinaryFile: public File {
public:
  BinaryFile();
  BinaryFile(std::vector<char> data);

  /*
  Reads data for this BinaryFile object from the specified file.
      <file_name>   - the file to read from.
  */
  void read(std::string file_name);

  /*
  Writes this BinaryFile object into the specified file.
      <file_name>   - the name of the file to write to.
  */
  void write(std::string file_name);

  /*accessors*/
  std::vector<char> get_data();

private:
  /*data*/
  std::vector<char> data;
};

#endif
