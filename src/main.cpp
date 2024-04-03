#include "instream.h"
#include "interpreter.h"
#include "loader.h"
#include "utils.h"
#include <fstream>
#include <iostream>
#include <vector>

int main(int argc, char **argv) {
  using namespace bf;

  std::vector<string> args;
  for (int i = 1; i < argc; i++)
    args.emplace_back(argv[i]);

  string filename;
  bool do_clean = true;
  int clean_file_flag = 0;
  string clean_filename = "cleaned.bf";
  int infile_flag = 0;
  int input_arg_flag = 0;
  bool calc_cost = false;

  InStream instream;

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
        instream.setup_file(arg);
      } else if (input_arg_flag == 1) {
        input_arg_flag = 2;
        instream.setup_string(arg);
      } else {
        filename = arg;
      }
    }
  }

  if (filename.empty()) {
    std::cout << "Please input Brainfuck file path!" << std::endl;
    exit(1);
  }
  string code = loader::read_code_file(filename);
  if (do_clean)
    code = loader::clean_code(code);

  if (clean_file_flag) {
    std::ofstream cleaned(clean_filename);
    if (do_clean)
      code = loader::split_lines(code);
    cleaned << code;
    cleaned.close();
  } else {
    Interpreter interpreter(instream);
    auto call = [&]() { interpreter.execute(code); };
    auto cost = utils::timer_with(call);
    if (calc_cost)
      std::cout << "\nCost: " << cost << "s" << std::endl;
  }

  return 0;
}
