#include "plugin/components/KeyboardObserver.hpp"
#include "util/win/VirtualKey.hpp"

#include <cstdint>

#include <Windows.h>

namespace z3lx::gfu {
KeyboardObserver::KeyboardObserver() noexcept = default;
KeyboardObserver::~KeyboardObserver() noexcept = default;

void KeyboardObserver::Update() {
    for (size_t i = 0; i < isKeyDown.size(); ++i) {
        const auto keyIndex = static_cast<uint8_t>(i);
        const auto key = util::VirtualKey { keyIndex };
        const bool isCurrentKeyDown =
            (GetAsyncKeyState(keyIndex) & 0x8000) != 0;
        auto isPreviousKeyDown = isKeyDown[keyIndex];

        if (isCurrentKeyDown) {
            if (isPreviousKeyDown) {
                Notify(OnKeyHold { key });
            } else {
                Notify(OnKeyDown { key });
            }
        } else {
            Notify(OnKeyUp { key });
        }

        isPreviousKeyDown = isCurrentKeyDown;
    }
}
} // namespace z3lx::gfu
