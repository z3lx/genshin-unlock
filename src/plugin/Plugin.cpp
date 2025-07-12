#include "plugin/Plugin.hpp"
#include "plugin/Config.hpp"
#include "plugin/components/CursorState.hpp"
#include "plugin/components/FovUnlocker.hpp"
#include "plugin/components/FpsUnlocker.hpp"
#include "plugin/components/PersistentObject.hpp"
#include "plugin/components/VirtualKeyState.hpp"
#include "plugin/components/WindowState.hpp"

#include <filesystem>
#include <ranges>
#include <utility>

namespace z3lx::plugin {
Plugin::Plugin(std::filesystem::path configFilePath)
    : configFilePath { std::move(configFilePath) } {}

Plugin::~Plugin() noexcept = default;

void Plugin::Start() {
    auto& configFile = GetComponent<PersistentObject<Config>>();
    configFile.SetFilePath(configFilePath);
    ApplyConfig();
}

void Plugin::Notify(const OnPersistentObjectChange<Config>& event) {
    ApplyConfig();
}

void Plugin::Notify(const OnVirtualKeyDown& event) {
    auto& fovUnlocker = GetComponent<FovUnlocker>();
    if (!fovUnlocker.IsHooked()) {
        return;
    }

    auto& configFile = GetComponent<PersistentObject<Config>>();
    Config& config = configFile.GetObject();

    if (event.key == config.unlockFovKey) {
        config.unlockFov = !config.unlockFov;
        fovUnlocker.Enable(config.unlockFov);
    } else if (!config.unlockFov) {
        return;
    } else if (event.key == config.nextFovPresetKey) {
        const auto it = std::ranges::find_if(
            config.fovPresets,
            [&](const int fovPreset) {
                return config.targetFov < fovPreset;
            }
        );
        config.targetFov = (it != config.fovPresets.end()) ?
            *it : config.fovPresets.front();
        fovUnlocker.SetTargetFov(config.targetFov);
    } else if (event.key == config.prevFovPresetKey) {
        const auto it = std::ranges::find_if(
            config.fovPresets | std::views::reverse,
            [&](const int fovPreset) {
                return config.targetFov > fovPreset;
            }
        );
        config.targetFov = (it != config.fovPresets.rend()) ?
            *it : config.fovPresets.back();
        fovUnlocker.SetTargetFov(config.targetFov);
    }

    configFile.TryWrite();
}

void Plugin::Notify(const OnCursorVisibilityChange& event) {
    UpdateHookState();
}

void Plugin::Notify(const OnWindowFocusChange& event) {
    UpdateHookState();
}

void Plugin::ApplyConfig() {
    auto& configFile = GetComponent<PersistentObject<Config>>();
    auto& fpsUnlocker = GetComponent<FpsUnlocker>();
    auto& fovUnlocker = GetComponent<FovUnlocker>();

    const Config& config = configFile.GetObject();
    fpsUnlocker.Enable(config.unlockFps);
    fpsUnlocker.SetTargetFps(config.targetFps);
    fovUnlocker.Enable(config.unlockFov);
    fovUnlocker.SetTargetFov(config.targetFov);
    fovUnlocker.SetSmoothing(config.fovSmoothing);
}

void Plugin::UpdateHookState() {
    auto& unlocker = GetComponent<FovUnlocker>();
    const auto& window = GetComponent<WindowState>();
    const auto& cursor = GetComponent<CursorState>();

    if (const bool shouldHook = window.IsFocused() && !cursor.IsVisible();
        unlocker.IsHooked() != shouldHook) {
        unlocker.Hook(shouldHook);
    }
}
} // namespace z3lx::plugin
