#pragma once

#include "plugin/interfaces/IComponent.hpp"

#include <bitset>
#include <cstdint>

namespace z3lx::gfu {
struct OnKeyDown {
    const uint8_t vKey;
};

struct OnKeyHold {
    const uint8_t vKey;
};

struct OnKeyUp {
    const uint8_t vKey;
};

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
