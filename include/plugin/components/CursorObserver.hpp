#pragma once

#include "plugin/interfaces/IComponent.hpp"

#include <optional>

namespace z3lx::gfu {
struct OnCursorVisibilityChange {
    const bool visible;
};

class CursorObserver final : public IComponent<
    CursorObserver, OnCursorVisibilityChange> {
public:
    CursorObserver() noexcept;
    ~CursorObserver() noexcept;

    void Update();

    [[nodiscard]] bool Visible() const noexcept;

private:
    std::optional<bool> wasCursorVisible;
};
} // namespace z3lx::gfu
