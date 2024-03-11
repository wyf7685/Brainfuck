#pragma once

#include "global.h"
#include <map>

namespace bf {

class Interpreter {
private:
  string code;
  std::map<size_t, size_t> loop_map;

public:
  Interpreter(string bf_code);
  void execute();
};

} // namespace bf