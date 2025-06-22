#include "plugin/components/CursorObserver.hpp"

#include <Windows.h>

namespace z3lx::gfu {
CursorObserver::CursorObserver() noexcept = default;
CursorObserver::~CursorObserver() noexcept = default;

void CursorObserver::Update() {
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

bool CursorObserver::Visible() const noexcept {
    return wasCursorVisible.value_or(true);
}
} // namespace z3lx::gfu
