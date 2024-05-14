#pragma once

#include <exception>
#include <string>

namespace bf {

using std::string;

class Exception : public std::exception {
  const char *const _msg;

public:
  Exception() : _msg(nullptr) {}
  Exception(char const *const msg) : _msg(msg) {}
  Exception(const string msg) : _msg(msg.c_str()) {}
  ~Exception() = default;
  const char *what() const noexcept override {
    return _msg ? _msg : "Unkown exception";
  }
};

} // namespace bf
