#include <fstream>
#include <iostream>
#include <string>

#include "loader.h"

namespace bf {
namespace Loader {

string read_code_file(const string &filename) {
  std::ifstream fin(filename.c_str());
  if (!fin.good()) {
    std::cout << "Error: Cannot open file '" << filename << "'" << std::endl;
    exit(4);
  }

  std::string code, line;
  size_t pos;
  while (std::getline(fin, line)) {
    // 移除注释
    if ((pos = line.find('#')) != std::string::npos)
      line = line.substr(0, pos);

    // 代码引用
    while ((pos = line.find('{')) != std::string::npos) {
      size_t endpos = line.find('}', pos);
      if (endpos == std::string::npos) {
        std::cout << "Error: Code reference tag mismatch" << std::endl;
        exit(3);
      }
      // 截取引用路径
      string refpath = line.substr(pos + 1, endpos - pos - 1);
      string refcode = read_code_file(refpath);
      // 插入引用代码
      line = line.substr(0, pos) + refcode + line.substr(endpos + 1);
    }

    code += line + "\n";
  }
  fin.close();
  return code;
}

#ifdef BF_DEBUG
static const string ValidCode = "><+-[],.%";
#else
static const string ValidCode = "><+-[],.";
#endif

string clean_code(const string &code) {
  string result;
  for (auto it = code.begin(); it != code.end(); ++it) {
    if (ValidCode.find(*it) != string::npos) {
      result.push_back(*it);
    }
  }

  size_t pos;
  while ((pos = result.find("><")) != string::npos)
    result.erase(result.begin() + pos, result.begin() + pos + 2);
  while ((pos = result.find("<>")) != string::npos)
    result.erase(result.begin() + pos, result.begin() + pos + 2);

  return result;
}

string split_lines(string code, size_t line_size) {
  string result;
  while (code.size() >= line_size) {
    result += code.substr(0, line_size) + "\n";
    code = code.substr(line_size);
  }
  result += code;
  return result;
}

} // namespace Loader

} // namespace bf