#include <fstream>
#include <string>

#include "loader.h"

namespace bf {
namespace loader {

string parse_line(string line) {
  size_t pos;
  // 移除注释
  if ((pos = line.find('#')) != string::npos) {
    line = line.substr(0, pos);
  }

  // 代码引用
  while ((pos = line.find('{')) != string::npos) {
    size_t endpos = line.find('}', pos);
    if (endpos == string::npos) {
      throw Exception("Error: Code reference tag mismatch");
    }
    // 截取引用路径
    string refpath = line.substr(pos + 1, endpos - pos - 1);
    string refcode = parse_file(refpath);
    // 插入引用代码
    line = line.substr(0, pos) + refcode + line.substr(endpos + 1);
  }

  return line;
}

string parse_string(string str) {
  string code;
  size_t pos;
  while ((pos = str.find('\n')) != string::npos) {
    string line = str.substr(0, pos);
    str = str.substr(pos + 1);

    code += parse_line(line) + '\n';
  }
  code += parse_line(str);
  return code;
}

string parse_file(const string &filename) {
  std::ifstream fin(filename.c_str());
  if (!fin.good()) {
    string msg = "Error: Cannot open file '" + filename + "'";
    throw Exception(msg.c_str());
  }

  string code, line;
  while (std::getline(fin, line))
    code += line + '\n';
  return parse_string(code);
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

} // namespace loader

} // namespace bf