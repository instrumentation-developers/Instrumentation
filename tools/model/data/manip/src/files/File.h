#ifndef FILE_H
#define FILE_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include <bitset>

#include "../util/Utils.h"
#include "../Constants.h"

class File {
public:

  virtual void read(std::string file_name) = 0;
  virtual void write(std::string file_name) = 0;

};

#endif
