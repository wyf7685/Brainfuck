#pragma once

#include "global.h"
#include <string>

namespace bf {
namespace InStream {

char read();
void setup_file(const string &filename);
void setup_arg(const string &arg);

} // namespace InStream
} // namespace bf