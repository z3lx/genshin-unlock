#pragma once

#include "plugin/interfaces/IComponent.hpp"
#include "util/win/VirtualKey.hpp"

#include <bitset>

namespace z3lx::gfu {
struct OnVirtualKeyDown {
    const util::VirtualKey key;
};

struct OnVirtualKeyHold {
    const util::VirtualKey key;
};

struct OnVirtualKeyUp {
    const util::VirtualKey key;
};

class VirtualKeyInput final : public IComponent<
    VirtualKeyInput, OnVirtualKeyDown, OnVirtualKeyHold, OnVirtualKeyUp> {
public:
    VirtualKeyInput() noexcept;
    ~VirtualKeyInput() noexcept;

    void Update();

    [[nodiscard]] bool KeyDown(util::VirtualKey key) const noexcept;

private:
    std::bitset<256> keyStates;
};
} // namespace z3lx::gfu
