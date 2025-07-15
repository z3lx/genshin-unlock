#pragma once

#include <type_traits>

namespace z3lx::util {
template <typename T> requires std::is_integral_v<T>
T CeilDiv(const T a, const T b) noexcept {
    return (a + b - 1) / b;
}
} // namespace z3lx::util
