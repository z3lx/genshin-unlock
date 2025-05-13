#include "plugin/components/KeyboardObserver.hpp"
#include "plugin/Events.hpp"
#include "utils/Windows.hpp"
#include "utils/log/Logger.hpp"

#include <algorithm>
#include <cstdint>
#include <exception>
#include <future>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Windows.h>

struct KeyboardObserver::Hook {
    static void Register(KeyboardObserver* instance);
    static void Unregister(KeyboardObserver* instance) noexcept;

private:
    static LRESULT CALLBACK KeyboardProc(
        int nCode, WPARAM wParam, LPARAM lParam) noexcept;
    static void SetHook();
    static void ClearHook() noexcept;

    static inline std::mutex mutex {};
    static inline std::thread thread {};
    static inline HHOOK hHook {};
    static inline std::vector<KeyboardObserver*> instances {};
    static inline std::unordered_map<uint8_t, bool> keyDownStates {};
};

void KeyboardObserver::Hook::Register(KeyboardObserver* instance) {
    std::lock_guard lock { mutex };
    if (const auto it = std::ranges::find(instances, instance);
        it != instances.end()) {
        return;
    }

    instances.reserve(instances.size() + 1);
    if (instances.empty()) {
        SetHook();
    }
    instances.emplace_back(instance);
}

void KeyboardObserver::Hook::Unregister(KeyboardObserver* instance) noexcept {
    std::lock_guard lock { mutex };
    const auto it = std::ranges::find(instances, instance);
    if (it == instances.end()) {
        return;
    }

    instances.erase(it);
    if (instances.empty()) {
        ClearHook();
    }
}

LRESULT CALLBACK KeyboardObserver::Hook::KeyboardProc(
    const int nCode, const WPARAM wParam, const LPARAM lParam) noexcept {
    const auto next = [nCode, wParam, lParam]() noexcept {
        return CallNextHookEx(hHook, nCode, wParam, lParam);
    };

    if (nCode != HC_ACTION) {
        return next();
    }

    Event event {};
    const auto keyboard = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
    const auto key = static_cast<uint8_t>(keyboard->vkCode);
    bool* isKeyDownPtr {};
    try {
        isKeyDownPtr = &keyDownStates[key];
    } catch (const std::exception& e) {
        LOG_E("Failed to get key state: {}", e.what());
        return next();
    }
    auto& isKeyDown = *isKeyDownPtr;

    switch (wParam) {
        case WM_KEYDOWN: case WM_SYSKEYDOWN: {
            if (isKeyDown) {
                event.emplace<OnKeyHold>(key);
            } else {
                event.emplace<OnKeyDown>(key);
            }
            isKeyDown = true;
            break;
        }
        case WM_KEYUP: case WM_SYSKEYUP: {
            event.emplace<OnKeyUp>(key);
            isKeyDown = false;
            break;
        }
        default: {
            return next();
        }
    }

    {
        std::lock_guard lock { mutex };
        for (const auto instance : instances) {
            std::lock_guard instanceLock { instance->mutex };
            instance->keyboardEvents.emplace_back(event);
        }
    }

    return next();
}

void KeyboardObserver::Hook::SetHook() {
    std::promise<void> promise {};
    auto future = promise.get_future();
    const auto task = [&promise]() {
        try {
            ThrowOnSystemError(hHook = SetWindowsHookEx(
                WH_KEYBOARD_LL, KeyboardProc, nullptr, 0
            ));
        } catch (...) {
            promise.set_exception(std::current_exception());
            return;
        }
        promise.set_value();

        MSG msg {};
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    };
    thread = std::thread { task };
    future.get();
}

void KeyboardObserver::Hook::ClearHook() noexcept {
    const auto threadId = GetThreadId(thread.native_handle());
    PostThreadMessage(threadId, WM_QUIT, 0, 0);
    UnhookWindowsHookEx(hHook);
    hHook = nullptr;
    thread.join();
}

KeyboardObserver::KeyboardObserver() try {
    Hook::Register(this);
} catch (const std::exception& e) {
    LOG_E("Failed to create KeyboardObserver: {}", e.what());
    throw;
}

KeyboardObserver::~KeyboardObserver() noexcept {
    Hook::Unregister(this);
}

void KeyboardObserver::Update() noexcept {
    std::lock_guard lock { mutex };
    for (const auto& event : keyboardEvents) {
        Notify(event);
    }
    keyboardEvents.clear();
}
