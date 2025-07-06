#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace z3lx::loader {
struct Config {
    std::filesystem::path gamePath {
        R"(C:\Program Files\HoYoPlay\games\Genshin Impact game\GenshinImpact.exe)"
    };
    std::wstring gameArgs;
    std::vector<std::filesystem::path> dllPaths {
        "plugin.dll"
    };
    bool suspendLoad { false };

    void Serialize(std::vector<uint8_t>& buffer);
    static Config Deserialize(const std::vector<uint8_t>& buffer);
};
} // namespace z3lx::loader
