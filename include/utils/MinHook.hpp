#pragma once

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
    void Enable();
    void Disable();

    Ret CallOriginal(Args... args) const;

private:
    bool enabled;
    void* target;
    void* original;
};

#include "utils/MinHookInl.hpp"
