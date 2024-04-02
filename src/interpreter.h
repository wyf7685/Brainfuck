#pragma once

#include "global.h"
#include <cstddef>
#include <cstdint>
#include <map>
#include <vector>

namespace bf {

using MemoryValue = uint16_t;
size_t const MemorySize = 1024;

class Interpreter {
public:
  string code;
  std::map<size_t, size_t> loop_map;
  std::vector<MemoryValue> memory;

  Interpreter(string bf_code);
  void execute();
};

} // namespace bf