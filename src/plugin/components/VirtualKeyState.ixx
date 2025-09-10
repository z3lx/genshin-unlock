export module plugin:VirtualKeyState;

import :ComponentBase;

import pwu;
import std;

namespace z3lx::plugin {
class VirtualKeyState final : public ComponentBase<VirtualKeyState> {
public:
    VirtualKeyState() noexcept;
    ~VirtualKeyState() noexcept;

    void Update() noexcept;

    [[nodiscard]] bool IsKeyDown(pwu::VirtualKey key) const noexcept;
    [[nodiscard]] bool IsKeyHeld(pwu::VirtualKey key) const noexcept;
    [[nodiscard]] bool IsKeyUp(pwu::VirtualKey key) const noexcept;

private:
    static constexpr auto keyCount = 256;
    std::bitset<keyCount> currentKeyStates;
    std::bitset<keyCount> previousKeyStates;
};
} // namespace z3lx::plugin
