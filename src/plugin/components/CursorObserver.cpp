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

    if (const bool isCurrentCursorVisible = cursorInfo.flags & CURSOR_SHOWING;
        isCurrentCursorVisible != isPreviousCursorVisible) {
        isPreviousCursorVisible = isCurrentCursorVisible;
        Notify(OnCursorVisibilityChange { isCurrentCursorVisible });
    }
}
} // namespace z3lx::gfu
