#pragma once

#include "plugin/interfaces/IComponent.hpp"

#include <optional>

namespace z3lx::plugin {
class CursorState final : public IComponent<CursorState> {
public:
    CursorState() noexcept;
    ~CursorState() noexcept;

    void Update() noexcept;

    [[nodiscard]] bool IsVisible() const noexcept;

private:
    std::optional<bool> isVisible;
};
} // namespace z3lx::plugin
