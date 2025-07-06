#include "plugin/components/CursorState.hpp"

#include <Windows.h>

namespace z3lx::plugin {
CursorState::CursorState() noexcept = default;
CursorState::~CursorState() noexcept = default;

void CursorState::Update() {
    CURSORINFO cursorInfo { .cbSize = sizeof(cursorInfo) };
    if (!GetCursorInfo(&cursorInfo)) {
        return;
    }

    if (const bool isVisible = (cursorInfo.flags & CURSOR_SHOWING);
        wasVisible != isVisible) {
        wasVisible = isVisible;
        Notify(OnCursorVisibilityChange { isVisible });
    }
}

bool CursorState::IsVisible() const noexcept {
    return wasVisible.value_or(true);
}
} // namespace z3lx::gfu
