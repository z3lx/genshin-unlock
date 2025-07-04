#pragma once

#include <concepts>
#include <iterator>

namespace z3lx::util {
template<typename ContainerType, typename ValueType>
concept Container = requires(ContainerType container) {
    typename ContainerType::value_type;
    requires std::same_as<typename ContainerType::value_type, ValueType>;
    std::begin(container);
    std::end(container);
};
} // namespace z3lx::util
