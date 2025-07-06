#include "plugin/components/VirtualKeyState.hpp"
#include "util/win/VirtualKey.hpp"

#include <cstdint>

#include <Windows.h>

namespace z3lx::plugin {
VirtualKeyState::VirtualKeyState() noexcept = default;
VirtualKeyState::~VirtualKeyState() noexcept = default;

void VirtualKeyState::Update() {
    for (size_t i = 0; i < keyStates.size(); ++i) {
        const auto keyIndex = static_cast<uint8_t>(i);
        const auto key = util::VirtualKey { keyIndex };
        const bool isKeyDown = (GetAsyncKeyState(keyIndex) & 0x8000) != 0;
        auto wasKeyDown = keyStates[keyIndex];

        if (isKeyDown) {
            if (wasKeyDown) {
                Notify(OnVirtualKeyHold { key });
            } else {
                Notify(OnVirtualKeyDown { key });
            }
        } else {
            Notify(OnVirtualKeyUp { key });
        }

        wasKeyDown = isKeyDown;
    }
}

bool VirtualKeyState::IsKeyDown(const util::VirtualKey key) const noexcept {
    return keyStates[static_cast<uint8_t>(key)];
}
} // namespace z3lx::plugin
