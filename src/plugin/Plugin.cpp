#include "plugin/Plugin.hpp"
#include "plugin/Events.hpp"
#include "plugin/components/ConfigManager.hpp"
#include "plugin/components/CursorObserver.hpp"
#include "plugin/components/KeyboardObserver.hpp"
#include "plugin/components/Unlocker.hpp"
#include "plugin/components/WindowObserver.hpp"
#include "utils/log/Logger.hpp"
#include "utils/win/Loader.hpp"
#include "utils/win/User.hpp"

#include <exception>
#include <ranges>
#include <variant>

Plugin::Plugin() noexcept
    : isUnlockerHooked { false }
    , isWindowFocused { true }
    , isCursorVisible { true } {};

Plugin::~Plugin() noexcept = default;

void Plugin::Start() noexcept try {
    SetComponent<Unlocker>();
    SetComponent<ConfigManager>(
        utils::GetCurrentModuleFilePath().parent_path() / "fov_config.json");
    SetComponent<WindowObserver>();
    SetComponent<CursorObserver>();
    SetComponent<KeyboardObserver>();

    targetWindows = utils::GetCurrentProcessWindows();
} catch (const std::exception& e) {
    LOG_E("Failed to start plugin: {}", e.what());
}

template <>
void Plugin::Handle(const OnConfigChange& event) noexcept {
    try {
        config = GetComponent<ConfigManager>().Read();
    } catch (const std::exception& e) {
        LOG_W("Failed to read config: {}", e.what());
    }

    auto& unlocker = GetComponent<Unlocker>();
    unlocker.SetEnable(config.enabled);
    unlocker.SetFieldOfView(config.fov);
    unlocker.SetSmoothing(config.smoothing);
}

template <>
void Plugin::Handle(const OnKeyDown& event) noexcept {
    const auto key = event.vKey;
    auto& [enabled, fov, fovPresets, smoothing,
        enableKey, nextKey, prevKey, dumpKey] = config;

    if (!isUnlockerHooked) {
        return;
    }

    auto& unlocker = GetComponent<Unlocker>();
    if (key == enableKey) {
        enabled = !enabled;
        unlocker.SetEnable(enabled);
    } else if (!enabled || isCursorVisible) {
        return;
    } else if (key == nextKey) {
        const auto it = std::ranges::find_if(
            fovPresets,
            [fov](const int fovPreset) { return fov < fovPreset; });
        fov = it != fovPresets.end() ? *it : fovPresets.front();
        unlocker.SetFieldOfView(fov);
    } else if (key == prevKey) {
        const auto it = std::ranges::find_if(
            fovPresets | std::views::reverse,
            [fov](const int fovPreset) { return fov > fovPreset; });
        fov = it != fovPresets.rend() ? *it : fovPresets.back();
        unlocker.SetFieldOfView(fov);
    } else if (key == dumpKey) {
        // TODO: plugin.unlocker.DumpBuffer();
    }
}

template <>
void Plugin::Handle(const OnCursorVisibilityChange& event) noexcept {
    isCursorVisible = event.isCursorVisible;
    ConsumeState();
}

template <>
void Plugin::Handle(const OnForegroundWindowChange& event) noexcept {
    isWindowFocused = std::ranges::contains(
        targetWindows, event.foregroundWindow);
    ConsumeState();
}

template <typename Event>
void Plugin::Handle(const Event& event) noexcept {};

void Plugin::ConsumeState() noexcept try {
    if (const bool value = isWindowFocused && !isCursorVisible;
        isUnlockerHooked != value) {
        GetComponent<Unlocker>().SetHook(value);
        isUnlockerHooked = value;
    }
} catch (const std::exception& e) {
    LOG_E("Failed to set hook: {}", e.what());
}

struct Plugin::Visitor {
    Plugin& plugin;

    template <typename Event>
    void operator()(const Event& event) const noexcept;
};

template <typename Event>
void Plugin::Visitor::operator()(const Event& event) const noexcept {
    plugin.Handle(event);
}

void Plugin::Notify(const Event& event) noexcept {
    std::visit(Visitor { *this }, event);
}
