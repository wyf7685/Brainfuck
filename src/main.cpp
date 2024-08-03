#include "instream.h"
#include "interpreter.h"
#include "loader.h"
#include "utils.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

static struct {
  std::string filename;
  bool do_clean = true;
  int clean_flag = 0;
  std::string clean_filename = "cleaned.bf";
  bool calc_cost = false;
  std::shared_ptr<bf::InStream> instream;
} arguments;

static void parse_args(int argc, char **argv) {
  int infile_flag = 0;
  int input_arg_flag = 0;
  arguments.instream = std::make_shared<bf::InStream>();

  for (int i = 1; i < argc; i++) {
    std::string arg(argv[i]);

    if (arg[0] == '-') {
      switch (arg[1]) {
      case 'n':
        arguments.do_clean = false;
        break;
      case 'o':
        arguments.clean_flag = 1;
        break;
      case 'i':
        infile_flag = 1;
        break;
      case 'p':
        input_arg_flag = 1;
        break;
      case 'c':
        arguments.calc_cost = true;
        break;
      default:
        break;
      }
      continue;
    } else if (arguments.clean_flag == 1) {
      arguments.clean_filename = arg;
      arguments.clean_flag = 2;
    } else if (infile_flag == 1) {
      infile_flag = 2;
      arguments.instream->setup_file(arg);
    } else if (input_arg_flag == 1) {
      input_arg_flag = 2;
      arguments.instream->setup_string(arg);
    } else {
      arguments.filename = arg;
    }
  }
}

int main(int argc, char **argv) {
  using namespace bf;

  parse_args(argc, argv);

  if (arguments.filename.empty()) {
    std::cout << "Please input Brainfuck file path!" << std::endl;
    exit(1);
  }
  std::string code = loader::parse_file(arguments.filename);
  if (arguments.do_clean)
    code = loader::clean_code(code);

  if (arguments.clean_flag) {
    std::ofstream cleaned(arguments.clean_filename);
    if (arguments.do_clean)
      code = loader::split_lines(code);
    cleaned << code;
    cleaned.close();
  } else {
    Interpreter interpreter(arguments.instream);
    auto call = [&interpreter, &code]() { interpreter.execute(code); };
    auto cost = utils::timer_with(call);
    if (arguments.calc_cost)
      std::cout << "\nCost: " << cost << "s" << std::endl;
  }

  return 0;
}
