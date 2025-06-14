#pragma once

#include "plugin/Events.hpp"
#include "plugin/interfaces/IComponent.hpp"

#include <optional>

namespace z3lx::gfu {
class CursorObserver final : public IComponent<Event> {
public:
    CursorObserver() noexcept;
    ~CursorObserver() noexcept override;

private:
    void Update() noexcept override;

    std::optional<bool> isPreviousCursorVisible;
};
} // namespace z3lx::gfu
