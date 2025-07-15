#pragma once

#include "plugin/interfaces/IComponent.hpp"
#include "util/win/VirtualKey.hpp"

#include <bitset>

namespace z3lx::plugin {
class VirtualKeyState final : public IComponent<VirtualKeyState> {
public:
    VirtualKeyState() noexcept;
    ~VirtualKeyState() noexcept;

    void Update() noexcept;

    [[nodiscard]] bool IsKeyDown(util::VirtualKey key) const noexcept;
    [[nodiscard]] bool IsKeyHeld(util::VirtualKey key) const noexcept;
    [[nodiscard]] bool IsKeyUp(util::VirtualKey key) const noexcept;

private:
    static constexpr auto keyCount = 256;
    std::bitset<keyCount> currentKeyStates;
    std::bitset<keyCount> previousKeyStates;
};
} // namespace z3lx::plugin
