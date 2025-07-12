#pragma once

#include "util/win/VirtualKey.hpp"

#include <cstdint>
#include <vector>

namespace z3lx::plugin {
struct Config {
    bool unlockFps = true;
    int targetFps = 120;

    bool unlockFov = true;
    int targetFov = 75;
    std::vector<int> fovPresets { 30, 45, 60, 75, 90, 110 };
    float fovSmoothing = 0.125;
    util::VirtualKey unlockFovKey = util::VirtualKey::DownArrow;
    util::VirtualKey nextFovPresetKey = util::VirtualKey::RightArrow;
    util::VirtualKey prevFovPresetKey = util::VirtualKey::LeftArrow;

    void Serialize(std::vector<uint8_t>& buffer);
    void Deserialize(const std::vector<uint8_t>& buffer);
};
} // namespace z3lx::plugin
