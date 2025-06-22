#include "plugin/components/WindowObserver.hpp"

#include <Windows.h>

namespace z3lx::gfu {
WindowObserver::WindowObserver() noexcept = default;
WindowObserver::~WindowObserver() noexcept = default;

void WindowObserver::Update() {
    const HWND foregroundWindow = GetForegroundWindow();
    if (!foregroundWindow) {
        return;
    }

    DWORD foregroundProcessId = 0;
    GetWindowThreadProcessId(foregroundWindow, &foregroundProcessId);
    if (!foregroundProcessId) {
        return;
    }

    const DWORD currentProcessId = GetCurrentProcessId();

    if (const bool isWindowFocused = (foregroundProcessId == currentProcessId);
        wasWindowFocused != isWindowFocused) {
        wasWindowFocused = isWindowFocused;
        Notify(OnWindowFocusChange { isWindowFocused });
    }
}

bool WindowObserver::Focused() const noexcept {
    return wasWindowFocused.value_or(false);
}
} // namespace z3lx::gfu
