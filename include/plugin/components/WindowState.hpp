#pragma once

#include "plugin/interfaces/IComponent.hpp"

#include <optional>

namespace z3lx::gfu {
struct OnWindowFocusChange {
    const bool focused;
};

class WindowState final : public IComponent<
    WindowState, OnWindowFocusChange> {
public:
    WindowState() noexcept;
    ~WindowState() noexcept;

    void Update();

    [[nodiscard]] bool Focused() const noexcept;

private:
    std::optional<bool> wasFocused;
};
} // namespace z3lx::gfu
