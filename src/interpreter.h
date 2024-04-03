#pragma once

#include "global.h"
#include "instream.h"

#include <cstddef>
#include <cstdint>
#include <map>
#include <vector>

namespace bf {

using MemoryValue = uint16_t;
size_t const MemorySize = 1024;

class Interpreter {
public:
  InStream instream;
  string code;
  std::map<size_t, size_t> loopMap;
  std::vector<MemoryValue> memory;

  Interpreter(InStream inStream, string bfCode);
  void execute();
};

} // namespace bf