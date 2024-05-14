#pragma once

#include <functional>

namespace bf {
namespace utils {

long double timer_with(std::function<void()> call);

} // namespace utils
} // namespace bf
