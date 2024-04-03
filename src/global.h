#pragma once

#include <exception>
#include <string>

namespace bf {

using std::string;

class Exception : public std::exception {
  char const *const _msg;

public:
  Exception() : _msg(nullptr) {}
  Exception(char const *const msg) : _msg(msg) {}
  char const *what() const noexcept override {
    return _msg ? _msg : "Unkown exception";
  }
};

} // namespace bf
