#pragma once

#include "plugin/interfaces/IComponent.hpp"

#include <optional>

namespace z3lx::gfu {
struct OnCursorVisibilityChange {
    const bool isCursorVisible;
};

class CursorObserver final : public IComponent<
    CursorObserver, OnCursorVisibilityChange> {
public:
    CursorObserver() noexcept;
    ~CursorObserver() noexcept;

    void Update();

private:
    std::optional<bool> isPreviousCursorVisible;
};
} // namespace z3lx::gfu
