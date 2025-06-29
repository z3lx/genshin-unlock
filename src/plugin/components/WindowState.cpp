#include "plugin/components/WindowState.hpp"

#include <Windows.h>

namespace z3lx::gfu {
WindowState::WindowState() noexcept = default;
WindowState::~WindowState() noexcept = default;

void WindowState::Update() {
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

    if (const bool isFocused = (foregroundProcessId == currentProcessId);
        wasFocused != isFocused) {
        wasFocused = isFocused;
        Notify(OnWindowFocusChange { isFocused });
    }
}

bool WindowState::Focused() const noexcept {
    return wasFocused.value_or(false);
}
} // namespace z3lx::gfu
