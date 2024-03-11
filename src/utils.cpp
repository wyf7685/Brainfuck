#include "../include/utils.h"
#include <chrono>

namespace bf {
namespace utils {

long double timer_with(std::function<void()> call) {
  using namespace std::chrono;
  auto start = steady_clock::now();
  call();
  auto end = steady_clock::now();

  auto duration = duration_cast<microseconds>(end - start);
  return static_cast<long double>(duration.count()) / 1000 / 1000;
}

} // namespace utils
} // namespace bf