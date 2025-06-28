#pragma once

#include "plugin/interfaces/IComponent.hpp"
#include "util/win/VirtualKey.hpp"

#include <bitset>

namespace z3lx::gfu {
struct OnKeyDown {
    const util::VirtualKey vKey;
};

struct OnKeyHold {
    const util::VirtualKey vKey;
};

struct OnKeyUp {
    const util::VirtualKey vKey;
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
