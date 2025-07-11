#pragma once

#include "util/win/VirtualKey.hpp"

#include <cstdint>
#include <vector>

namespace z3lx::plugin {
struct Config {
    bool fpsEnabled = true;
    int fpsOverride = 120;

    bool fovEnabled = true;
    int fovOverride = 75;
    std::vector<int> fovPresets { 30, 45, 60, 75, 90, 110 };
    float fovSmoothing = 0.125;
    util::VirtualKey fovEnableKey = util::VirtualKey::DownArrow;
    util::VirtualKey fovNextPresetKey = util::VirtualKey::RightArrow;
    util::VirtualKey fovPrevPresetKey = util::VirtualKey::LeftArrow;

    void Serialize(std::vector<uint8_t>& buffer);
    void Deserialize(const std::vector<uint8_t>& buffer);
};
} // namespace z3lx::plugin
