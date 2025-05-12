#pragma once

#include "utils/MinHook.hpp"

#include <MinHook.h>

#include <cstddef>
#include <mutex>
#include <stdexcept>

namespace MinHookImpl {
inline std::mutex mutex {};
inline size_t referenceCount { 0 };

inline void ThrowMhStatus(const MH_STATUS status) {
    throw std::runtime_error { MH_StatusToString(status) };
}

inline void ThrowOnMhError(const MH_STATUS status) {
    if (status != MH_OK) {
        ThrowMhStatus(status);
    }
}
} // namespace MinHookImpl

template <typename Ret, typename... Args>
MinHook<Ret, Args...>::MinHook(void* target, void* detour, const bool enable)
    : enabled { enable }
    , target { target }
    , original { nullptr } {
    using namespace MinHookImpl;
    std::lock_guard lock { mutex };

    const auto handleMhError = [](const MH_STATUS status) {
        if (status != MH_OK) {
            MH_Uninitialize();
            --referenceCount;
            ThrowMhStatus(status);
        }
    };

    // Initialize MinHook
    if (referenceCount++ == 0) {
        handleMhError(MH_Initialize());
    }

    // Create and enable the hook
    handleMhError(MH_CreateHook(target, detour, &original));
    if (enable) {
        handleMhError(MH_EnableHook(target));
    }
}

template <typename Ret, typename... Args>
MinHook<Ret, Args...>::~MinHook() noexcept {
    using namespace MinHookImpl;
    std::lock_guard lock { mutex };

    if (target == nullptr) {
        return;
    }

    MH_RemoveHook(target);
    if (--referenceCount == 0) {
        MH_Uninitialize();
    }
}

template <typename Ret, typename... Args>
MinHook<Ret, Args...>::MinHook(MinHook&& other) noexcept
    : enabled { other.enabled }
    , target { other.target }
    , original { other.original } {
    other.enabled = false;
    other.target = nullptr;
    other.original = nullptr;
}

template <typename Ret, typename... Args>
MinHook<Ret, Args...>&
MinHook<Ret, Args...>::operator=(MinHook&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    enabled = other.enabled;
    target = other.target;
    original = other.original;

    other.enabled = false;
    other.target = nullptr;
    other.original = nullptr;

    return *this;
}

template <typename Ret, typename... Args>
bool MinHook<Ret, Args...>::IsEnabled() const noexcept {
    return enabled;
}

template <typename Ret, typename... Args>
void MinHook<Ret, Args...>::Enable() {
    using namespace MinHookImpl;
    if (!enabled) {
        ThrowOnMhError(MH_EnableHook(target));
        enabled = true;
    }
}

template <typename Ret, typename... Args>
void MinHook<Ret, Args...>::Disable() {
    using namespace MinHookImpl;
    if (enabled) {
        ThrowOnMhError(MH_DisableHook(target));
        enabled = false;
    }
}

template <typename Ret, typename... Args>
Ret MinHook<Ret, Args...>::CallOriginal(Args... args) const {
    using FuncPtr = Ret(*)(Args...);
    return reinterpret_cast<FuncPtr>(original)(args...);
}
