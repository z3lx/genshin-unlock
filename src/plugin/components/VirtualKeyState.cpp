#include "plugin/components/VirtualKeyState.hpp"
#include "util/win/VirtualKey.hpp"

#include <cstdint>

#include <Windows.h>

namespace z3lx::plugin {
VirtualKeyState::VirtualKeyState() noexcept = default;
VirtualKeyState::~VirtualKeyState() noexcept = default;

void VirtualKeyState::Update() noexcept {
    previousKeyStates = currentKeyStates;
    for (size_t i = 0; i < keyCount; ++i) {
        const auto keyIndex = static_cast<uint8_t>(i);
        const bool isKeyDown = GetAsyncKeyState(keyIndex) & 0x8000;
        currentKeyStates[keyIndex] = isKeyDown;
    }
}

bool VirtualKeyState::IsKeyDown(const util::VirtualKey key) const noexcept {
    const auto keyIndex = static_cast<uint8_t>(key);
    return currentKeyStates[keyIndex] && !previousKeyStates[keyIndex];
}

bool VirtualKeyState::IsKeyHeld(const util::VirtualKey key) const noexcept {
    const auto keyIndex = static_cast<uint8_t>(key);
    return currentKeyStates[keyIndex];
}

bool VirtualKeyState::IsKeyUp(const util::VirtualKey key) const noexcept {
    const auto keyIndex = static_cast<uint8_t>(key);
    return !currentKeyStates[keyIndex] && previousKeyStates[keyIndex];
}
} // namespace z3lx::plugin
