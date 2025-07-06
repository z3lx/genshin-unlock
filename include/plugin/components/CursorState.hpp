#pragma once

#include "plugin/interfaces/IComponent.hpp"

#include <optional>

namespace z3lx::plugin {
struct OnCursorVisibilityChange {
    const bool isVisible;
};

class CursorState final : public IComponent<
    CursorState, OnCursorVisibilityChange> {
public:
    CursorState() noexcept;
    ~CursorState() noexcept;

    void Update();

    [[nodiscard]] bool IsVisible() const noexcept;

private:
    std::optional<bool> wasVisible;
};
} // namespace z3lx::gfu
