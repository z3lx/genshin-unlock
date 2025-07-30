#pragma once

#include "plugin/interfaces/IComponent.hpp"

#include <optional>

namespace z3lx::plugin {
class WindowState final : public IComponent<WindowState> {
public:
    WindowState() noexcept;
    ~WindowState() noexcept;

    void Update() noexcept;

    [[nodiscard]] bool IsFocused() const noexcept;

private:
    std::optional<bool> isFocused;
};
} // namespace z3lx::plugin
