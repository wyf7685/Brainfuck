#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <vector>

namespace bf {
using std::string;
using std::vector;

namespace InStream {
enum class StreamType {
  Stdin = 0,
  File,
};
static StreamType Type = StreamType::Stdin;
static std::string InputFile;
static size_t InputFileIndex;

char read_stdin() {
  if (std::cin.eof())
    return '\0';
  char byte;
  std::cin.read(&byte, 1);
  return byte;
}

char read_file() {
  if (InputFileIndex == InputFile.size())
    return '\0';
  return InputFile[InputFileIndex++];
}

char read() {
  switch (Type) {
  case StreamType::Stdin:
    return read_stdin();
  case StreamType::File:
    return read_file();
  default:
    return '\0';
  }
}

void setup_file(string filename) {
  std::ifstream file(filename);
  if (!file.good()) {
    std::cout << "Error: Cannot open file '" << filename << "'" << std::endl;
    exit(4);
  }
  string line;
  while (std::getline(file, line)) {
    InputFile += line + "\n";
  }
  InputFileIndex = 0;
  Type = StreamType::File;
}

void setup_arg(const string &arg) {
  InputFile = arg;
  InputFileIndex = 0;
  Type = StreamType::File;
}

} // namespace InStream

string read_code_file(const string &filename) {
  std::ifstream fin(filename.c_str());
  if (!fin.good()) {
    std::cout << "Error: Cannot open file '" << filename << "'" << std::endl;
    exit(4);
  }

  std::string code, line;
  while (std::getline(fin, line)) {
    size_t pos;
    // 移除注释
    if ((pos = line.find('#')) != std::string::npos) {
      line = line.substr(0, pos);
    }
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

static const string ValidCode = "><+-[],.%";

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

string split_lines(string code) {
  string result;
  while (code.size() >= 70) {
    result += code.substr(0, 70) + "\n";
    code = code.substr(70);
  }
  result += code;
  return result;
}

std::map<size_t, size_t> get_loop_map(const string &code) {
  std::stack<size_t> loop_stack;
  std::map<size_t, size_t> loop_map;
  for (size_t i = 0; i < code.length(); ++i) {
    if (code[i] == '[') {
      loop_stack.push(i);
    } else if (code[i] == ']') {
      loop_map[loop_stack.top()] = i;
      loop_stack.pop();
    }
  }
  if (!loop_stack.empty()) {
    std::cout << "Error: Loop tag mismatch\n";
    exit(2);
  }
  return loop_map;
}

void execute_code(const string &code) {
  std::map<size_t, size_t> loop_map = get_loop_map(code);
  std::stack<size_t> loop_stack;
  vector<uint16_t> memory(128, 0);
  size_t pointer = 0;
  size_t idx = 0;

  while (idx < code.size()) {
    switch (code[idx]) {
    case '>': {
      size_t i = 0;
      while (code[idx + i] == '>')
        i++;
      idx += i - 1;
      pointer += i;
      while (pointer >= memory.size())
        memory.emplace_back(0);
    } break;
    case '<': {
      size_t i = 0;
      while (code[idx + i] == '<')
        i++;
      idx += i - 1;
      pointer -= i;
    } break;
    case '+': {
      size_t i = 0;
      while (code[idx + i] == '+')
        i++;
      idx += i - 1;
      memory[pointer] += i;
    } break;
    case '-': {
      size_t i = 0;
      while (code[idx + i] == '-')
        i++;
      idx += i - 1;
      memory[pointer] -= i;
    } break;
    case '.':
      std::cout << static_cast<char>(memory[pointer]);
      std::cout.flush();
      break;
    case ',':
      memory[pointer] = InStream::read();
      break;
    case '[':
      if (code[idx + 1] == '-' && code[idx + 2] == ']') {
        memory[pointer] = 0;
        idx += 2;
        break;
      }
      if (memory[pointer]) {
        loop_stack.push(idx);
      } else {
        idx = loop_map.at(idx);
      }
      break;
    case ']':
      if (memory[pointer]) {
        idx = loop_stack.top() - 1;
      }
      loop_stack.pop();
      break;
    case '%': {
      std::cout << "\nDebug: " << pointer << " [";
      for (size_t i = 0; i < 15; i++) {
        std::cout << memory[i] << ", ";
      }
      std::cout << memory[15] << "]" << std::endl;
    } break;
    }
    idx++;
  }
}

inline long double timer_with(std::function<void()> call) {
  using namespace std::chrono;
  auto start = steady_clock::now();
  call();
  auto end = steady_clock::now();

  auto duration = duration_cast<microseconds>(end - start);
  return static_cast<long double>(duration.count()) / 1000 / 1000;
}
} // namespace bf

int main(int argc, char **argv) {
  using namespace bf;
  vector<string> args;
  for (int i = 1; i < argc; i++)
    args.emplace_back(argv[i]);

  string filename;
  bool do_clean = true;
  int clean_file_flag = 0;
  string clean_filename = "cleaned.bf";
  int infile_flag = 0;
  int input_arg_flag = 0;
  bool calc_cost = false;

  for (const string &arg : args) {
    if (arg[0] == '-') {
      switch (arg[1]) {
      case 'n':
        do_clean = false;
        break;
      case 'o':
        clean_file_flag = 1;
        break;
      case 'i':
        infile_flag = 1;
        break;
      case 'p':
        input_arg_flag = 1;
        break;
      case 'c':
        calc_cost = true;
        break;
      default:
        break;
      }
    } else {
      if (clean_file_flag == 1) {
        clean_filename = arg;
        clean_file_flag = 2;
      } else if (infile_flag == 1) {
        infile_flag = 2;
        InStream::setup_file(arg);
      } else if (input_arg_flag == 1) {
        input_arg_flag = 2;
        InStream::setup_arg(arg);
      } else {
        filename = arg;
      }
    }
  }

  if (filename.empty()) {
    std::cout << "Please input Brainfuck file path!" << std::endl;
    exit(1);
  }
  string code = read_code_file(filename);
  if (do_clean)
    code = clean_code(code);

  if (clean_file_flag) {
    std::ofstream cleaned(clean_filename);
    if (do_clean)
      code = split_lines(code);
    cleaned << code;
    cleaned.close();
  } else {
    auto cost = timer_with([&code]() { execute_code(code); });
    if (calc_cost)
      std::cout << "\nCost: " << cost << "s" << std::endl;
  }

  return 0;
}
