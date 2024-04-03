#pragma once

#include "global.h"
#include "instream.h"

#include <cstddef>
#include <cstdint>
#include <map>
#include <vector>

namespace bf {

using MemoryValue = uint16_t;
static const size_t MemorySize = 64;

class Interpreter {
public:
  InStream inStream;
  std::vector<MemoryValue> memory;

  Interpreter() : inStream(InStream()){};
  Interpreter(InStream inStream);
  void execute(const string &code);
};

} // namespace bf