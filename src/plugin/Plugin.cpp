#include "plugin/Plugin.hpp"
#include "plugin/Events.hpp"
#include "plugin/components/ConfigManager.hpp"
#include "plugin/components/Unlocker.hpp"
#include "util/win/Loader.hpp"
#include "util/win/User.hpp"

#include <wil/result.h>

#include <filesystem>
#include <ranges>
#include <variant>

namespace z3lx::gfu {
Plugin::Plugin()
    : isWindowFocused { true }
    , isCursorVisible { true } {};

Plugin::~Plugin() noexcept = default;

void Plugin::Start() {
    GetComponent<ConfigManager>().FilePath(
        util::GetCurrentModuleFilePath().parent_path() / "fov_config.json"
    );
    targetWindows = util::GetCurrentProcessWindows();
}

template <>
void Plugin::Handle(const OnConfigChange& event) {
    try {
        config = GetComponent<ConfigManager>().Read();
    } CATCH_LOG()

    auto& unlocker = GetComponent<Unlocker>();
    unlocker.Enabled(config.enabled);
    unlocker.FieldOfView(config.fov);
    unlocker.Smoothing(config.smoothing);
}

template <>
void Plugin::Handle(const OnKeyDown& event) {
    const auto key = event.vKey;
    auto& [enabled, fov, fovPresets, smoothing,
        enableKey, nextKey, prevKey] = config;

    auto& unlocker = GetComponent<Unlocker>();
    if (!unlocker.Hooked()) {
        return;
    }

    if (key == enableKey) {
        enabled = !enabled;
        unlocker.Enabled(enabled);
    } else if (!enabled || isCursorVisible) {
        return;
    } else if (key == nextKey) {
        const auto it = std::ranges::find_if(
            fovPresets,
            [fov](const int fovPreset) {
                return fov < fovPreset;
            }
        );
        fov = it != fovPresets.end() ? *it : fovPresets.front();
        unlocker.FieldOfView(fov);
    } else if (key == prevKey) {
        const auto it = std::ranges::find_if(
            fovPresets | std::views::reverse,
            [fov](const int fovPreset) {
                return fov > fovPreset;
            }
        );
        fov = it != fovPresets.rend() ? *it : fovPresets.back();
        unlocker.FieldOfView(fov);
    }
}

template <>
void Plugin::Handle(const OnCursorVisibilityChange& event) {
    isCursorVisible = event.isCursorVisible;
    ConsumeState();
}

template <>
void Plugin::Handle(const OnForegroundWindowChange& event) {
    isWindowFocused = std::ranges::contains(
        targetWindows, event.foregroundWindow);
    ConsumeState();
}

template <typename Event>
void Plugin::Handle(const Event& event) {};

void Plugin::ConsumeState() {
    auto& unlocker = GetComponent<Unlocker>();
    if (const bool value = isWindowFocused && !isCursorVisible;
        unlocker.Hooked() != value) {
        unlocker.Hooked(value);
    }
}

void Plugin::Notify(const Event& event) {
    const auto visitor = [this](const auto& event) -> void {
        Handle(event);
    };
    std::visit(visitor, event);
}
} // namespace z3lx::gfu
