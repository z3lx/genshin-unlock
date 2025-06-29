#include "plugin/components/CursorState.hpp"

#include <Windows.h>

namespace z3lx::gfu {
CursorState::CursorState() noexcept = default;
CursorState::~CursorState() noexcept = default;

void CursorState::Update() {
    CURSORINFO cursorInfo { .cbSize = sizeof(cursorInfo) };
    if (!GetCursorInfo(&cursorInfo)) {
        return;
    }

    if (const bool isCursorVisible = (cursorInfo.flags & CURSOR_SHOWING);
        wasCursorVisible != isCursorVisible) {
        wasCursorVisible = isCursorVisible;
        Notify(OnCursorVisibilityChange { isCursorVisible });
    }
}

bool CursorState::Visible() const noexcept {
    return wasCursorVisible.value_or(true);
}
} // namespace z3lx::gfu
