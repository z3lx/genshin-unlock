#pragma once

#include <concepts>
#include <ranges>

namespace z3lx::util {
template<typename ContainerType, typename ValueType>
concept Container =
    std::ranges::input_range<ContainerType> &&
    std::same_as<std::ranges::range_value_t<ContainerType>, ValueType>;
} // namespace z3lx::util
