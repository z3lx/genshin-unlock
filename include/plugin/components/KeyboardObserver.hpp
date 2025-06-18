#pragma once

#include "plugin/Events.hpp"
#include "plugin/interfaces/IComponent.hpp"

#include <bitset>

namespace z3lx::gfu {
class KeyboardObserver final : public IComponent<Event> {
public:
    KeyboardObserver() noexcept;
    ~KeyboardObserver() noexcept override;

private:
    void Update() override;

    std::bitset<256> isKeyDown;
};
} // namespace z3lx::gfu
