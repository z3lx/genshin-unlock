#pragma once

#include "plugin/interfaces/IComponent.hpp"

#include <optional>

namespace z3lx::gfu {
struct OnCursorVisibilityChange {
    const bool visible;
};

class CursorState final : public IComponent<
    CursorState, OnCursorVisibilityChange> {
public:
    CursorState() noexcept;
    ~CursorState() noexcept;

    void Update();

    [[nodiscard]] bool Visible() const noexcept;

private:
    std::optional<bool> wasCursorVisible;
};
} // namespace z3lx::gfu
