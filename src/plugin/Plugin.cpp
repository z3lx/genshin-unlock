#include "plugin/Plugin.hpp"
#include "plugin/Config.hpp"
#include "plugin/components/CursorState.hpp"
#include "plugin/components/PersistentObject.hpp"
#include "plugin/components/Unlocker.hpp"
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
    auto& unlocker = GetComponent<Unlocker>();
    if (!unlocker.IsHooked()) {
        return;
    }

    auto& configFile = GetComponent<PersistentObject<Config>>();
    Config& config = configFile.GetObject();

    if (event.key == config.enableKey) {
        config.enabled = !config.enabled;
        unlocker.Enable(config.enabled);
    } else if (!config.enabled) {
        return;
    } else if (event.key == config.nextKey) {
        const auto it = std::ranges::find_if(
            config.fovPresets,
            [&](const int fovPreset) {
                return config.fov < fovPreset;
            }
        );
        config.fov = (it != config.fovPresets.end()) ?
            *it : config.fovPresets.front();
        unlocker.SetFieldOfView(config.fov);
    } else if (event.key == config.prevKey) {
        const auto it = std::ranges::find_if(
            config.fovPresets | std::views::reverse,
            [&](const int fovPreset) {
                return config.fov > fovPreset;
            }
        );
        config.fov = (it != config.fovPresets.rend()) ?
            *it : config.fovPresets.back();
        unlocker.SetFieldOfView(config.fov);
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
    auto& unlocker = GetComponent<Unlocker>();
    auto& configFile = GetComponent<PersistentObject<Config>>();

    const Config& config = configFile.GetObject();
    unlocker.Enable(config.enabled);
    unlocker.SetFieldOfView(config.fov);
    unlocker.SetSmoothing(config.smoothing);
}

void Plugin::UpdateHookState() {
    auto& unlocker = GetComponent<Unlocker>();
    const auto& window = GetComponent<WindowState>();
    const auto& cursor = GetComponent<CursorState>();

    if (const bool shouldHook = window.IsFocused() && !cursor.IsVisible();
        unlocker.IsHooked() != shouldHook) {
        unlocker.Hook(shouldHook);
    }
}
} // namespace z3lx::plugin
