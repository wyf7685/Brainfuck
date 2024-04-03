#include "interpreter.h"
#include "instream.h"
#include <iostream>
#include <map>
#include <stack>
#include <vector>

namespace bf {

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

Interpreter::Interpreter(InStream inStream) {
  this->inStream = inStream;
  this->memory = std::vector<MemoryValue>(MemorySize, 0);
}

void Interpreter::execute(string code) {
  std::map<size_t, size_t> loopMap = get_loop_map(code);
  std::stack<size_t> loop_stack;
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
      memory[pointer] = inStream.read();
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
        idx = loopMap.at(idx);
      }
      break;
    case ']':
      if (memory[pointer]) {
        idx = loop_stack.top() - 1;
      }
      loop_stack.pop();
      break;
#ifdef BF_DEBUG
    case '%': {
      std::cout << "\nDebug: " << pointer << " [";
      for (size_t i = 0; i < 15; i++) {
        if (pointer == i)
          std::cout << "*";
        std::cout << memory[i] << ", ";
      }
      std::cout << memory[15] << "]\n";
      std::cout.flush();
    } break;
#endif // BF_DEBUG
    }
    idx++;
  }
}

} // namespace bf