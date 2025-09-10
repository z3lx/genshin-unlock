module;

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

module plugin;

import :WindowState;

namespace z3lx::plugin {
WindowState::WindowState() noexcept = default;
WindowState::~WindowState() noexcept = default;

void WindowState::Update() noexcept {
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
    isFocused = foregroundProcessId == currentProcessId;
}

bool WindowState::IsFocused() const noexcept {
    return isFocused.value_or(false);
}
} // namespace z3lx::plugin
