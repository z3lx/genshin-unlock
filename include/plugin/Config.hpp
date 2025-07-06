#pragma once

#include "util/win/VirtualKey.hpp"

#include <cstdint>
#include <vector>

namespace z3lx::plugin {
struct Config {
    bool enabled = true;
    uint8_t fov = 75;
    std::vector<uint8_t> fovPresets { 30, 45, 60, 75, 90, 110 };
    float smoothing = 0.125;
    util::VirtualKey enableKey = util::VirtualKey::DownArrow;
    util::VirtualKey nextKey = util::VirtualKey::RightArrow;
    util::VirtualKey prevKey = util::VirtualKey::LeftArrow;

    void Serialize(std::vector<uint8_t>& buffer);
    void Deserialize(const std::vector<uint8_t>& buffer);
};
} // namespace z3lx::gfu
