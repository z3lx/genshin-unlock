#pragma once

#include <cstdint>

namespace z3lx::plugin {
template <typename Derived, uint16_t Frequency = 60>
class IRunnable {
public:
    [[noreturn]] void Run();
};
} // namespace z3lx::plugin

#include "plugin/interfaces/IRunnableInl.hpp"
