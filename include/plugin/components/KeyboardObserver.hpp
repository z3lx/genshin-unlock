#pragma once

#include "plugin/Events.hpp"
#include "plugin/interfaces/IComponent.hpp"

#include <bitset>

namespace z3lx::gfu {
class KeyboardObserver final : public IComponent<
    KeyboardObserver, OnKeyDown, OnKeyHold, OnKeyUp> {
public:
    KeyboardObserver() noexcept;
    ~KeyboardObserver() noexcept;

    void Update();

private:
    std::bitset<256> isKeyDown;
};
} // namespace z3lx::gfu
