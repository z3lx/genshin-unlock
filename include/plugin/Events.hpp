#pragma once

#include <cstdint>
#include <Windows.h>

namespace z3lx::gfu {
struct OnConfigChange {};

struct OnKeyDown {
    const uint8_t vKey;
};

struct OnKeyHold {
    const uint8_t vKey;
};

struct OnKeyUp {
    const uint8_t vKey;
};

struct OnCursorVisibilityChange {
    const bool isCursorVisible;
};

struct OnForegroundWindowChange {
    const HWND foregroundWindow;
};
} // namespace z3lx::gfu
