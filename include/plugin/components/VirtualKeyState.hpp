#pragma once

#include "plugin/interfaces/IComponent.hpp"
#include "util/win/VirtualKey.hpp"

#include <bitset>

namespace z3lx::plugin {
struct OnVirtualKeyDown {
    const util::VirtualKey key;
};

struct OnVirtualKeyHold {
    const util::VirtualKey key;
};

struct OnVirtualKeyUp {
    const util::VirtualKey key;
};

class VirtualKeyState final : public IComponent<
    VirtualKeyState, OnVirtualKeyDown, OnVirtualKeyHold, OnVirtualKeyUp> {
public:
    VirtualKeyState() noexcept;
    ~VirtualKeyState() noexcept;

    void Update();

    [[nodiscard]] bool IsKeyDown(util::VirtualKey key) const noexcept;

private:
    std::bitset<256> keyStates;
};
} // namespace z3lx::plugin
