#pragma once

#include "global.h"
#include "instream.h"

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <vector>

namespace bf {

using MemoryValue = uint16_t;
static const size_t MemorySize = 64;

class Interpreter {
public:
  std::shared_ptr<InStream> inStream;
  std::vector<MemoryValue> memory;

  Interpreter() : inStream(std::make_shared<InStream>()){};
  Interpreter(std::shared_ptr<InStream> inStream);
  void execute(const string &code);
};

} // namespace bf
