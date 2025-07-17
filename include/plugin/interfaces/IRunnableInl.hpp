#pragma once

#include "plugin/interfaces/IRunnable.hpp"
#include "util/Type.hpp"

#include <wil/result.h>

#include <chrono>
#include <cstdint>
#include <thread>

#define IRUNNABLE_TEMPLATE                                                      \
    template <typename Derived, uint16_t Frequency>

#define IRUNNABLE                                                               \
    z3lx::plugin::IRunnable<Derived, Frequency>

namespace z3lx::plugin {
IRUNNABLE_TEMPLATE
void IRUNNABLE::Run() try {
    auto derived = static_cast<Derived*>(this);
    derived->StartComponent();
    while (true) {
        derived->UpdateComponent();
        if constexpr (Frequency != 0) {
            using Milliseconds = std::chrono::duration<double, std::milli>;
            constexpr Milliseconds duration { 1000.0 / Frequency };
            std::this_thread::sleep_for(duration);
        }
    }
} CATCH_LOG_MSG(
    "From Run method in %hs",
    util::GetTypeName<IRunnable>()
)
} // namespace z3lx::plugin

#undef IRUNNABLE_TEMPLATE
#undef IRUNNABLE
