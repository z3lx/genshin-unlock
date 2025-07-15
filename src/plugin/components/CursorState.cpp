#include "plugin/components/CursorState.hpp"

#include <Windows.h>

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
