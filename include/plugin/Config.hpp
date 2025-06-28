#pragma once

#include "util/win/VirtualKey.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace z3lx::gfu {
struct Config {
    bool enabled = true;
    uint8_t fov = 75;
    std::vector<uint8_t> fovPresets {
        30, 45, 60, 75, 90, 110
    };
    float smoothing = 0.125;

    util::VirtualKey enableKey = util::VirtualKey::DownArrow;
    util::VirtualKey nextKey = util::VirtualKey::RightArrow;
    util::VirtualKey prevKey = util::VirtualKey::LeftArrow;

    void Serialize(std::string& buffer);
    void Deserialize(const std::string& buffer);
};
} // namespace z3lx::gfu
