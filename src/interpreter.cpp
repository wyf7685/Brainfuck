#include "../include/interpreter.h"
#include "../include/instream.h"
#include <cstdint>
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

Interpreter::Interpreter(string bf_code) {
  code = bf_code;
  loop_map = get_loop_map(code);
}

void Interpreter::execute() {
  std::stack<size_t> loop_stack;
  std::vector<uint16_t> memory(128, 0);
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
#ifdef BF_DEBUG
    case '%': {
      std::cout << "\nDebug: " << pointer << " [";
      for (size_t i = 0; i < 15; i++) {
        if (pointer == i)
          std::cout << "*";
        std::cout << memory[i] << ", ";
      }
      std::cout << memory[15] << "]" << std::endl;
    } break;
#endif // BF_DEBUG
    }
    idx++;
  }
}

} // namespace bf