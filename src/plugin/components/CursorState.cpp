module;

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

module plugin;

import :CursorState;

namespace z3lx::plugin {
CursorState::CursorState() noexcept = default;
CursorState::~CursorState() noexcept = default;

void CursorState::Update() noexcept {
    CURSORINFO cursorInfo { .cbSize = sizeof(cursorInfo) };
    if (!GetCursorInfo(&cursorInfo)) {
        return;
    }
    isVisible = cursorInfo.flags & CURSOR_SHOWING;
}

bool CursorState::IsVisible() const noexcept {
    return isVisible.value_or(true);
}
} // namespace z3lx::plugin
