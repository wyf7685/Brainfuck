#pragma once

#include "global.h"

namespace bf {
namespace Loader {

string read_code_file(const string &filename);
string clean_code(const string &code);
string split_lines(string code, int line_size = 60);

} // namespace Loader
} // namespace bf