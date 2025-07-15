#include "plugin/Plugin.hpp"
#include "plugin/Config.hpp"
#include "plugin/components/FovUnlocker.hpp"
#include "plugin/components/FpsUnlocker.hpp"
#include "plugin/components/PersistentObject.hpp"
#include "plugin/components/VirtualKeyState.hpp"
#include "util/win/Loader.hpp"

#include <algorithm>
#include <filesystem>
#include <ranges>

namespace z3lx::plugin {
Plugin::Plugin() = default;
Plugin::~Plugin() noexcept = default;

void Plugin::Start() {
    auto& configFile = GetComponent<PersistentObject<Config>>();
    const std::filesystem::path currentPath =
        util::GetCurrentModuleFilePath().parent_path();
    configFile.SetFilePath(currentPath / "plugin_config.json");
}

void Plugin::Update() {
    const auto& virtualKey = GetComponent<VirtualKeyState>();
    auto& fpsUnlocker = GetComponent<FpsUnlocker>();
    auto& fovUnlocker = GetComponent<FovUnlocker>();
    auto& configFile = GetComponent<PersistentObject<Config>>();

    // Update FPS Unlocker
    fpsUnlocker.Enable(configFile.Get(&Config::unlockFps));
    fpsUnlocker.SetTargetFps(configFile.Get(&Config::targetFps));

    // Update FOV unlocker
    if (!fovUnlocker.IsHooked()) {
        return;
    }

    if (virtualKey.IsKeyDown(configFile.Get(&Config::unlockFovKey))) {
        constexpr auto unlockFov = &Config::unlockFov;
        const bool value = configFile.Get(unlockFov);
        configFile.Set(unlockFov, !value);
    } else if (!configFile.Get(&Config::unlockFov)) {
        return;
    } else if (virtualKey.IsKeyDown(configFile.Get(&Config::nextFovPresetKey))) {
        constexpr auto targetFov = &Config::targetFov;
        const auto& fovPresets = configFile.Get(&Config::fovPresets);
        const auto it = std::ranges::find_if(
            fovPresets,
            [&](const int fovPreset) {
                return configFile.Get(targetFov) < fovPreset;
            }
        );
        configFile.Set(
            targetFov,
            (it != fovPresets.end()) ? *it : fovPresets.front()
        );
    } else if (virtualKey.IsKeyDown(configFile.Get(&Config::prevFovPresetKey))) {
        constexpr auto targetFov = &Config::targetFov;
        const auto& fovPresets = configFile.Get(&Config::fovPresets);
        const auto it = std::ranges::find_if(
            fovPresets | std::views::reverse,
            [&](const int fovPreset) {
                return configFile.Get(targetFov) > fovPreset;
            }
        );
        configFile.Set(
            targetFov,
            (it != fovPresets.rend()) ? *it : fovPresets.back()
        );
    }

    fovUnlocker.Enable(configFile.Get(&Config::unlockFov));
    fovUnlocker.SetTargetFov(configFile.Get(&Config::targetFov));
    fovUnlocker.SetSmoothing(configFile.Get(&Config::fovSmoothing));
}
} // namespace z3lx::plugin
