#pragma once

#include <functional>

namespace bf::utils {

long double timer_with(std::function<void()> call);

} // namespace bf::utils

