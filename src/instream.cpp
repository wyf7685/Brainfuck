#include <fstream>
#include <iostream>
#include <string>

#include "instream.h"

namespace bf {

void InStream::setup_stdin() { type = StreamType::Stdin; }

void InStream::setup_file(const string &filename) {
  std::ifstream file(filename);
  if (!file.good()) {
    throw Exception("Error: Cannot open file '" + filename + "'");
  }
  string line;
  while (std::getline(file, line)) {
    input += line + "\n";
  }
  input_index = 0;
  type = StreamType::File;
}

void InStream::setup_string(const string &arg) {
  input = arg + "\n";
  input_index = 0;
  type = StreamType::File;
}

char InStream::read() {
  switch (type) {
  case StreamType::Stdin: {
    if (std::cin.eof())
      return '\0';
    char byte;
    std::cin.read(&byte, 1);
    return byte;
  }
  case StreamType::File: {
    if (input_index == input.size())
      return '\0';
    return input[input_index++];
  }
  default:
    return '\0';
  }
}

} // namespace bf
