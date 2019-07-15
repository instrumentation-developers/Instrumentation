#include <vector>
#include <string>
#include <iostream>
#include <dirent.h>

#include "../util/Command.h"
#include "../Constants.h"
//converisons.
void btoi(unsigned char* src, int size, unsigned int & dest);
void btoi(char* src, int size, int & dest);
void itob(unsigned int src, unsigned char* dest, int size);
void itob(int src, char* dest, int size);

//file management.
std::vector<std::string> open_dir(std::string path);
