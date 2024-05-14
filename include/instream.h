#pragma once

#include "common.h"
#include <string>

namespace bf {

enum class StreamType : int {
  Stdin = 0,
  File,
};

class InStream {
  StreamType type = StreamType::Stdin;
  std::string input;
  size_t input_index = 0;

public:
  InStream() = default;
  InStream(const InStream &other) = default;
  ~InStream() = default;

  void setup_stdin();
  void setup_file(const string &filename);
  void setup_string(const string &str);
  char read();
};

} // namespace bf
