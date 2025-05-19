#include "plugin/components/ConfigManager.hpp"
#include "utils/log/Logger.hpp"

#include <nlohmann/json.hpp>
#include <wil/filesystem.h>
#include <wil/result.h>

#include <algorithm>
#include <atomic>
#include <exception>
#include <filesystem>
#include <fstream>
#include <memory>
#include <utility>

namespace {
constexpr auto ENABLED = "enabled";
constexpr auto FOV = "fov";
constexpr auto FOV_PRESETS = "fov_presets";
constexpr auto SMOOTHING = "smoothing";
constexpr auto ENABLE_KEY = "enable_key";
constexpr auto NEXT_KEY = "next_key";
constexpr auto PREV_KEY = "prev_key";
constexpr auto DUMP_KEY = "dump_key";
} // namespace

ConfigManager::ConfigManager(std::filesystem::path filepath) try
    : filepath { std::move(filepath) }
    , changed { true } {
    reader = wil::make_folder_change_reader(
        this->filepath.parent_path().wstring().c_str(), false,
        wil::FolderChangeEvents::LastWriteTime,
        [this](const auto event, const auto fileName) {
            OnFolderChange(event, fileName);
        });
} CATCH_THROW_NORMALIZED()

ConfigManager::~ConfigManager() noexcept = default;

#define TRY_GET_TO(json, key, value)                                            \
    try {                                                                       \
        (json).at(key).get_to(value);                                           \
    } catch (const std::exception& e) {                                         \
        LOG_W("Failed to parse key '{}': {}", key, e.what());                   \
    }

#define TRY_GET_TO_IF(json, key, value, condition)                              \
    try {                                                                       \
        auto previousValue = value;                                             \
        (json).at(key).get_to(value);                                           \
        if (!(condition)) {                                                     \
            (value) = previousValue;                                            \
            LOG_W(                                                              \
                "Invalid value for key '{}': {} is not satisfied",              \
                key, #condition                                                 \
            );                                                                  \
        }                                                                       \
    } catch (const std::exception& e) {                                         \
        LOG_W("Failed to parse key '{}': {}", key, e.what());                   \
    }

Config ConfigManager::Read() const {
    std::ifstream file { filepath };
    if (!file.is_open()) {
        throw std::runtime_error { "Failed to open file" };
    }

    nlohmann::ordered_json j {};
    file >> j;

    Config config {};
    auto& [enabled, fov, fovPresets, smoothing,
        enableKey, nextKey, prevKey, dumpKey] = config;

    TRY_GET_TO(j, ENABLED, enabled);
    TRY_GET_TO_IF(j, FOV, fov,
        fov > 0 && fov < 180);
    TRY_GET_TO_IF(j, FOV_PRESETS, fovPresets,
        std::ranges::all_of(fovPresets, [](const int fovPreset) {
            return fovPreset > 0 && fovPreset < 180;
        }));
    TRY_GET_TO_IF(j, SMOOTHING, smoothing,
        smoothing >= 0.0 && smoothing <= 1.0);
    TRY_GET_TO_IF(j, ENABLE_KEY, enableKey,
        enableKey > 0 && enableKey < 255);
    TRY_GET_TO_IF(j, NEXT_KEY, nextKey,
        nextKey > 0 && nextKey < 255);
    TRY_GET_TO_IF(j, PREV_KEY, prevKey,
        prevKey > 0 && prevKey < 255);
    TRY_GET_TO_IF(j, DUMP_KEY, dumpKey,
        dumpKey > 0 && dumpKey < 255);

    std::ranges::sort(fovPresets);
    const auto last = std::ranges::unique(fovPresets).begin();
    fovPresets.erase(last, fovPresets.end());

    return config;
}

void ConfigManager::Write(const Config& config) const {
    auto& [enabled, fov, fovPresets, smoothing,
        enableKey, nextKey, prevKey, dumpKey] = config;

    const nlohmann::ordered_json j {
        { ENABLED, enabled },
        { FOV, fov },
        { FOV_PRESETS, fovPresets },
        { SMOOTHING, smoothing },
        { ENABLE_KEY, enableKey },
        { NEXT_KEY, nextKey },
        { PREV_KEY, prevKey },
        { DUMP_KEY, dumpKey }
    };

    std::ofstream file { filepath };
    if (!file.is_open()) {
        throw std::runtime_error { "Failed to open file" };
    }
    file << j.dump(4);
}

void ConfigManager::OnFolderChange(
    const wil::FolderChangeEvent event, const PCWSTR filename) noexcept {
    if (event == wil::FolderChangeEvent::Modified &&
        filename == filepath.filename()) {
        changed.store(true, std::memory_order_relaxed);
    }
}

void ConfigManager::Update() noexcept {
    if (changed.load(std::memory_order_relaxed)) {
        changed.store(false, std::memory_order_relaxed);
        Notify(OnConfigChange {});
    }
}
