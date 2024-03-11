#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>

#include "../include/instream.h"

namespace bf {
using std::string;

namespace InStream {
enum class StreamType {
  Stdin = 0,
  File,
};
static StreamType Type = StreamType::Stdin;
static std::string InputFile;
static size_t InputFileIndex;

char read_stdin() {
  if (std::cin.eof())
    return '\0';
  char byte;
  std::cin.read(&byte, 1);
  return byte;
}

char read_file() {
  if (InputFileIndex == InputFile.size())
    return '\0';
  return InputFile[InputFileIndex++];
}

char read() {
  switch (Type) {
  case StreamType::Stdin:
    return read_stdin();
  case StreamType::File:
    return read_file();
  default:
    return '\0';
  }
}

void setup_file(const string &filename) {
  std::ifstream file(filename);
  if (!file.good()) {
    std::cout << "Error: Cannot open file '" << filename << "'" << std::endl;
    exit(4);
  }
  string line;
  while (std::getline(file, line)) {
    InputFile += line + "\n";
  }
  InputFileIndex = 0;
  Type = StreamType::File;
}

void setup_arg(const string &arg) {
  InputFile = arg;
  InputFileIndex = 0;
  Type = StreamType::File;
}

} // namespace InStream
} // namespace bf
