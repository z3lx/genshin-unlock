#include "plugin/components/WindowState.hpp"

#include <Windows.h>

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
