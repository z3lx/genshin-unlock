#pragma once

#include "plugin/Events.hpp"
#include "plugin/interfaces/IComponent.hpp"

#include <Windows.h>

namespace z3lx::gfu {
class WindowObserver final : public IComponent<Event> {
public:
    WindowObserver() noexcept;
    ~WindowObserver() noexcept override;

private:
    void Update() override;

    HWND previousForegroundWindow;
};
} // namespace z3lx::gfu
