#include "Utils.h"

void btoi(unsigned char* src, int size, unsigned int & dest) {
  dest = src[0];

  for (unsigned int i = 1; i < size; i++) {
    dest = dest | (src[i] << (i * BITS_PER_BYTE));
  }
}

void btoi(char* src, int size, int & dest) {
  dest = src[0];

  for (unsigned int i = 1; i < size; i++) {
    dest = dest | (src[i] << (i * BITS_PER_BYTE));
  }
}

void itob(unsigned int src, unsigned char* dest, int size) {
  for (unsigned int i = 0; i < size; i++) {
    dest[i] = src >> (i * BITS_PER_BYTE);
  }
}

void itob(int src, char* dest, int size) {
  for (unsigned int i = 0; i < size; i++) {
    dest[i] = src >> (i * BITS_PER_BYTE);
  }
}

std::vector<std::string> open_dir(std::string path) {
    DIR*    dir;
    dirent* pdir;

    std::vector<std::string> files;
    dir = opendir(path.c_str());

    while (pdir = readdir(dir)) {
        files.push_back(pdir -> d_name);
    }

    files.erase(files.begin());
    files.erase(files.begin());

    closedir(dir);

    return files;
}
