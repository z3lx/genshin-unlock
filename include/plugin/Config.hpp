#pragma once

#include "util/win/VirtualKey.hpp"

#include <cstdint>
#include <vector>

namespace z3lx::plugin {
struct Config {
    bool unlockFps = true;
    int targetFps = 120;
    bool autoThrottle = true;

    bool unlockFov = true;
    int targetFov = 90;
    std::vector<int> fovPresets { 30, 45, 60, 75, 90, 110 };
    float fovSmoothing = 0.125;
    util::VirtualKey unlockFovKey = util::VirtualKey::DownArrow;
    util::VirtualKey nextFovPresetKey = util::VirtualKey::RightArrow;
    util::VirtualKey prevFovPresetKey = util::VirtualKey::LeftArrow;

    bool fixBurstFov = false;
    int burstFovDelayMs = 1600;
    util::VirtualKey burstFovKey = util::VirtualKey::Q;

    void Serialize(std::vector<uint8_t>& buffer);
    void Deserialize(const std::vector<uint8_t>& buffer);
};
} // namespace z3lx::plugin
