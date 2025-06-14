#pragma once

#include <cstdint>
#include <variant>

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

using Event = std::variant<
    OnConfigChange,
    OnKeyDown,
    OnKeyHold,
    OnKeyUp,
    OnCursorVisibilityChange,
    OnForegroundWindowChange
>;
} // namespace z3lx::gfu
