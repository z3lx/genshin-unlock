#pragma once

#include <cstdint>

namespace z3lx::plugin {
template <typename Derived, uint16_t Frequency = 100>
class IRunnable {
public:
    IRunnable() noexcept;
    ~IRunnable() noexcept;
    IRunnable(const IRunnable&) = delete;
    IRunnable(IRunnable&&) = delete;

    [[noreturn]] void Run();
};
} // namespace z3lx::plugin

#include "plugin/interfaces/IRunnableInl.hpp"
