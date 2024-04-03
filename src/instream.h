#pragma once

#include "global.h"
#include <string>

namespace bf {

enum class StreamType {
  Stdin = 0,
  File,
};

class InStream {
  StreamType type = StreamType::Stdin;
  std::string input;
  size_t input_index = 0;

public:
  InStream() = default;
  ~InStream() = default;

  bool setup_stdin();
  bool setup_file(const string &filename);
  bool setup_arg(const string &arg);
  char read();
};

} // namespace bf
