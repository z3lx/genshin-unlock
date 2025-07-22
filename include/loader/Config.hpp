#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace z3lx::loader {
enum class DisplayMode : uint8_t {
    Windowed = 0,
    Fullscreen = 1,
    Borderless = 2
};

struct Config {
    bool checkUpdates = true;
    std::filesystem::path gamePath {
        LR"(C:\Program Files\HoYoPlay\games\Genshin Impact game\GenshinImpact.exe)"
    };
    bool overrideArgs = false;
    uint8_t monitorIndex = 1;
    DisplayMode displayMode = DisplayMode::Fullscreen;
    uint16_t screenWidth = 1920;
    uint16_t screenHeight = 1080;
    bool mobilePlatform = false;
    std::string additionalArgs {};

    std::vector<std::filesystem::path> dllPaths {
        "plugin.dll"
    };
    bool suspendLoad = false;

    void Serialize(std::vector<uint8_t>& buffer);
    void Deserialize(const std::vector<uint8_t>& buffer);
};
} // namespace z3lx::loader
