#include "plugin/Plugin.hpp"
#include "plugin/Config.hpp"
#include "plugin/components/CursorState.hpp"
#include "plugin/components/PersistentObject.hpp"
#include "plugin/components/Unlocker.hpp"
#include "plugin/components/VirtualKeyState.hpp"
#include "plugin/components/WindowState.hpp"
#include "util/win/Loader.hpp"

#include <filesystem>
#include <ranges>

namespace z3lx::gfu {
Plugin::Plugin() = default;
Plugin::~Plugin() noexcept = default;

void Plugin::Start() {
    GetComponent<PersistentObject<Config>>().FilePath(
        util::GetCurrentModuleFilePath().parent_path() / "fov_config.json"
    );
}

void Plugin::Notify(const OnPersistentObjectChange<Config>& event) {
    const Config& config = event.object;
    auto& unlocker = GetComponent<Unlocker>();
    unlocker.Enabled(config.enabled);
    unlocker.FieldOfView(config.fov);
    unlocker.Smoothing(config.smoothing);
}

void Plugin::Notify(const OnVirtualKeyDown& event) {
    auto& unlocker = GetComponent<Unlocker>();
    if (!unlocker.Hooked()) {
        return;
    }

    Config& config = GetComponent<PersistentObject<Config>>().Object();
    if (event.key == config.enableKey) {
        config.enabled = !config.enabled;
        unlocker.Enabled(config.enabled);
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
        unlocker.FieldOfView(config.fov);
    } else if (event.key == config.prevKey) {
        const auto it = std::ranges::find_if(
            config.fovPresets | std::views::reverse,
            [&](const int fovPreset) {
                return config.fov > fovPreset;
            }
        );
        config.fov = (it != config.fovPresets.rend()) ?
            *it : config.fovPresets.back();
        unlocker.FieldOfView(config.fov);
    }
}

void Plugin::Notify(const OnCursorVisibilityChange& event) {
    UpdateHookState();
}

void Plugin::Notify(const OnWindowFocusChange& event) {
    UpdateHookState();
}

void Plugin::UpdateHookState() {
    auto& unlocker = GetComponent<Unlocker>();
    const auto& window = GetComponent<WindowState>();
    const auto& cursor = GetComponent<CursorState>();

    if (const bool shouldHook = window.Focused() && !cursor.Visible();
        unlocker.Hooked() != shouldHook) {
        unlocker.Hooked(shouldHook);
    }
}
} // namespace z3lx::gfu
