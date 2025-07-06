#pragma once

#include "plugin/interfaces/IComponent.hpp"

#include <optional>

namespace z3lx::plugin {
struct OnWindowFocusChange {
    const bool isFocused;
};

class WindowState final : public IComponent<
    WindowState, OnWindowFocusChange> {
public:
    WindowState() noexcept;
    ~WindowState() noexcept;

    void Update();

    [[nodiscard]] bool IsFocused() const noexcept;

private:
    std::optional<bool> wasFocused;
};
} // namespace z3lx::gfu
