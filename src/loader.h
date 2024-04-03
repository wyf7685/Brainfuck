#pragma once

#include "global.h"

namespace bf {
namespace loader {

string parse_string(string str);
string parse_file(const string &filename);
string clean_code(const string &code);
string split_lines(string code, size_t line_size = 60);

} // namespace loader
} // namespace bf