#pragma once

#include "plugin/Events.hpp"
#include "plugin/interfaces/IComponent.hpp"

#include <Windows.h>

namespace z3lx::gfu {
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
