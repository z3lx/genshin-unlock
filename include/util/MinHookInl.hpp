#pragma once

#include "util/MinHook.hpp"

#include <MinHook.h>

#include <cstddef>
#include <mutex>
#include <stdexcept>

namespace z3lx::util {
namespace detail::minhook_impl {
inline std::mutex mutex {};
inline size_t referenceCount { 0 };

inline void ThrowStatus(const MH_STATUS status) {
    throw std::runtime_error { MH_StatusToString(status) };
}

inline void ThrowOnError(const MH_STATUS status) {
    if (status != MH_OK) {
        ThrowStatus(status);
    }
}
} // namespace detail::minhook_impl

template <typename Ret, typename... Args>
MinHook<Ret, Args...>::MinHook(
    void* target, void* detour, const bool enable)
    : isEnabled { enable }
    , target { target }
    , original { nullptr } {
    using namespace detail::minhook_impl;
    std::lock_guard lock { mutex };

    const auto handleMhError = [](const MH_STATUS status) {
        if (status != MH_OK) {
            MH_Uninitialize();
            --referenceCount;
            ThrowStatus(status);
        }
    };

    // Initialize MinHook
    if (referenceCount++ == 0) {
        handleMhError(MH_Initialize());
    }

    // Create and enable the hook
    handleMhError(MH_CreateHook(target, detour, &original));
    if (isEnabled) {
        handleMhError(MH_EnableHook(target));
    }
}

template <typename Ret, typename... Args>
MinHook<Ret, Args...>::~MinHook() noexcept {
    using namespace detail::minhook_impl;
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
    : isEnabled { other.isEnabled }
    , target { other.target }
    , original { other.original } {
    other.isEnabled = false;
    other.target = nullptr;
    other.original = nullptr;
}

template <typename Ret, typename... Args>
MinHook<Ret, Args...>&
MinHook<Ret, Args...>::operator=(MinHook&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    isEnabled = other.isEnabled;
    target = other.target;
    original = other.original;

    other.isEnabled = false;
    other.target = nullptr;
    other.original = nullptr;

    return *this;
}

template <typename Ret, typename... Args>
bool MinHook<Ret, Args...>::IsEnabled() const noexcept {
    return isEnabled;
}

template <typename Ret, typename... Args>
void MinHook<Ret, Args...>::Enable(const bool enable) {
    using namespace detail::minhook_impl;
    if (enable && !isEnabled) {
        ThrowOnError(MH_EnableHook(target));
    } else if (!enable && isEnabled) {
        ThrowOnError(MH_DisableHook(target));
    }
    isEnabled = enable;
}

template <typename Ret, typename... Args>
Ret MinHook<Ret, Args...>::CallOriginal(Args... args) const {
    using FuncPtr = Ret(*)(Args...);
    return reinterpret_cast<FuncPtr>(original)(args...);
}
} // namespace z3lx::util
