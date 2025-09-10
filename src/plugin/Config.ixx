export module plugin:Config;

import pwu;
import std;

namespace z3lx::plugin {
struct Config {
    bool unlockFps = true;
    int targetFps = 120;
    bool autoThrottle = true;

    bool unlockFov = true;
    int targetFov = 90;
    std::vector<int> fovPresets { 30, 45, 60, 75, 90, 110 };
    float fovSmoothing = 0.125;
    pwu::VirtualKey unlockFovKey = pwu::VirtualKey::DownArrow;
    pwu::VirtualKey nextFovPresetKey = pwu::VirtualKey::RightArrow;
    pwu::VirtualKey prevFovPresetKey = pwu::VirtualKey::LeftArrow;

    void Serialize(std::vector<std::uint8_t>& buffer);
    void Deserialize(const std::vector<std::uint8_t>& buffer);
};
} // namespace z3lx::plugin
