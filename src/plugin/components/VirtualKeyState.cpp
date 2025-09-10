module;

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

module plugin;

import :VirtualKeyState;

import pwu;
import std;

namespace z3lx::plugin {
VirtualKeyState::VirtualKeyState() noexcept = default;
VirtualKeyState::~VirtualKeyState() noexcept = default;

void VirtualKeyState::Update() noexcept {
    previousKeyStates = currentKeyStates;
    for (size_t i = 0; i < keyCount; ++i) {
        const auto keyIndex = static_cast<std::uint8_t>(i);
        const bool isKeyDown = GetAsyncKeyState(keyIndex) & 0x8000;
        currentKeyStates[keyIndex] = isKeyDown;
    }
}

bool VirtualKeyState::IsKeyDown(const pwu::VirtualKey key) const noexcept {
    const auto keyIndex = static_cast<std::uint8_t>(key);
    return currentKeyStates[keyIndex] && !previousKeyStates[keyIndex];
}

bool VirtualKeyState::IsKeyHeld(const pwu::VirtualKey key) const noexcept {
    const auto keyIndex = static_cast<std::uint8_t>(key);
    return currentKeyStates[keyIndex];
}

bool VirtualKeyState::IsKeyUp(const pwu::VirtualKey key) const noexcept {
    const auto keyIndex = static_cast<std::uint8_t>(key);
    return !currentKeyStates[keyIndex] && previousKeyStates[keyIndex];
}
} // namespace z3lx::plugin
