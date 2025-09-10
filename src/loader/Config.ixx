export module loader:Config;

import std;

export namespace z3lx::loader {
enum class DisplayMode : std::uint8_t {
    Windowed = 0,
    Fullscreen = 1,
    Borderless = 2
};

struct Config {
    std::filesystem::path gamePath {};
    bool overrideArgs = false;
    std::uint8_t monitorIndex = 1;
    DisplayMode displayMode = DisplayMode::Fullscreen;
    std::uint16_t screenWidth = 1920;
    std::uint16_t screenHeight = 1080;
    bool mobilePlatform = false;
    std::string additionalArgs {};

    std::vector<std::filesystem::path> dllPaths {
        "plugin.dll"
    };
    bool suspendLoad = false;

    void Serialize(std::vector<std::uint8_t>& buffer);
    void Deserialize(const std::vector<std::uint8_t>& buffer);
};
} // namespace z3lx::loader
