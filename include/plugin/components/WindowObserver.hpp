#pragma once

#include "plugin/interfaces/IComponent.hpp"

#include <Windows.h>

namespace z3lx::gfu {
struct OnForegroundWindowChange {
    const HWND foregroundWindow;
};

class WindowObserver final : public IComponent<
    WindowObserver, OnForegroundWindowChange> {
public:
    WindowObserver() noexcept;
    ~WindowObserver() noexcept;

    void Update();

private:
    HWND previousForegroundWindow;
};
} // namespace z3lx::gfu
