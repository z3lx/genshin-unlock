module;

// workaround to compile error
#include <strsafe.h>

module plugin;

import :Plugin;
import :Config;
import :FovUnlocker;
import :FpsUnlocker;
import :PersistentObject;
import :VirtualKeyState;

import std;

namespace z3lx::plugin {
Plugin::Plugin(std::filesystem::path configFilePath)
    : configFilePath { std::move(configFilePath) } {}

Plugin::~Plugin() noexcept = default;

void Plugin::Start() {
    auto& configFile = GetComponent<PersistentObject<Config>>();
    configFile.SetFilePath(std::move(configFilePath));
}

void Plugin::Update() {
    using C = Config;

    const auto& virtualKey = GetComponent<VirtualKeyState>();
    auto& fpsUnlocker = GetComponent<FpsUnlocker>();
    auto& fovUnlocker = GetComponent<FovUnlocker>();
    auto& configFile = GetComponent<PersistentObject<C>>();

    // Update FPS Unlocker
    fpsUnlocker.Enable(configFile.Get<&C::unlockFps>());
    fpsUnlocker.SetTargetFps(configFile.Get<&C::targetFps>());
    fpsUnlocker.AutoThrottle(configFile.Get<&C::autoThrottle>());

    // Update FOV unlocker
    if (!fovUnlocker.IsHooked()) {
        return;
    }

    if (virtualKey.IsKeyDown(configFile.Get<&C::unlockFovKey>())) {
        const bool value = configFile.Get<&C::unlockFov>();
        configFile.Set<&C::unlockFov>(!value);
    } else if (!configFile.Get<&C::unlockFov>()) {
        return;
    } else if (virtualKey.IsKeyDown(configFile.Get<&C::nextFovPresetKey>())) {
        const auto& fovPresets = configFile.Get<&C::fovPresets>();
        const auto it = std::ranges::find_if(
            fovPresets,
            [&](const int fovPreset) {
                return configFile.Get<&C::targetFov>() < fovPreset;
            }
        );
        configFile.Set<&C::targetFov>(
            (it != fovPresets.end()) ? *it : fovPresets.front()
        );
    } else if (virtualKey.IsKeyDown(configFile.Get<&C::prevFovPresetKey>())) {
        const auto& fovPresets = configFile.Get<&C::fovPresets>();
        const auto it = std::ranges::find_if(
            fovPresets | std::views::reverse,
            [&](const int fovPreset) {
                return configFile.Get<&C::targetFov>() > fovPreset;
            }
        );
        configFile.Set<&C::targetFov>(
            (it != fovPresets.rend()) ? *it : fovPresets.back()
        );
    }

    fovUnlocker.Enable(configFile.Get<&C::unlockFov>());
    fovUnlocker.SetTargetFov(configFile.Get<&C::targetFov>());
    fovUnlocker.SetSmoothing(configFile.Get<&C::fovSmoothing>());
}
} // namespace z3lx::plugin
