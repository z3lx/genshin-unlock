#pragma once

#include "plugin/interfaces/IComponent.hpp"

#include <optional>

namespace z3lx::gfu {
struct OnWindowFocusChange {
    const bool focused;
};

class WindowObserver final : public IComponent<
    WindowObserver, OnWindowFocusChange> {
public:
    WindowObserver() noexcept;
    ~WindowObserver() noexcept;

    void Update();

    [[nodiscard]] bool Focused() const noexcept;

private:
    std::optional<bool> wasWindowFocused;
};
} // namespace z3lx::gfu
