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
    : enabled { enable }, target { target }, original { nullptr } {
    using namespace MinHookImpl;
    const auto handleMhError = [](const MH_STATUS status) {
        if (status != MH_OK) {
            MH_Uninitialize();
            --referenceCount;
            ThrowMhStatus(status);
        }
    };

    std::lock_guard lock { mutex };

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

    MH_RemoveHook(target);
    if (--referenceCount == 0) {
        MH_Uninitialize();
    }
}

template <typename Ret, typename... Args>
bool MinHook<Ret, Args...>::IsEnabled() const noexcept {
    using namespace MinHookImpl;
    std::lock_guard lock { mutex };

    return enabled;
}

template <typename Ret, typename... Args>
void MinHook<Ret, Args...>::Enable() {
    using namespace MinHookImpl;
    std::lock_guard lock { mutex };

    if (!enabled) {
        ThrowOnMhError(MH_EnableHook(target));
        enabled = true;
    }
}

template <typename Ret, typename... Args>
void MinHook<Ret, Args...>::Disable() {
    using namespace MinHookImpl;
    std::lock_guard lock { mutex };

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
