#pragma once

namespace z3lx::util {
template <typename Ret, typename... Args>
class MinHook {
public:
    MinHook(void* target, void* detour, bool enable = false);
    ~MinHook() noexcept;

    MinHook(const MinHook& other) = delete;
    MinHook& operator=(const MinHook& other) = delete;
    MinHook(MinHook&& other) noexcept;
    MinHook& operator=(MinHook&& other) noexcept;

    [[nodiscard]] bool IsEnabled() const noexcept;
    void Enable(bool enable);

    Ret CallOriginal(Args... args) const;

private:
    bool isEnabled;
    void* target;
    void* original;
};
} // namespace z3lx::util

#include "util/MinHookInl.hpp"
