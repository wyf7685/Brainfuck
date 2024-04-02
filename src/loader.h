#pragma once

#include "global.h"

namespace bf {
namespace Loader {

string read_code_file(const string &filename);
string clean_code(const string &code);
string split_lines(string code, size_t line_size = 60);

} // namespace Loader
} // namespace bf