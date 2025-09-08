#pragma once

#include "plugin/interfaces/ComponentBase.hpp"

#include <optional>

namespace z3lx::plugin {
class CursorState final : public ComponentBase<CursorState> {
public:
    CursorState() noexcept;
    ~CursorState() noexcept;

    void Update() noexcept;

    [[nodiscard]] bool IsVisible() const noexcept;

private:
    std::optional<bool> isVisible;
};
} // namespace z3lx::plugin
