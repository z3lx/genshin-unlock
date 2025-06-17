#include "plugin/components/ConfigManager.hpp"
#include "util/win/File.hpp"

#include <nlohmann/json.hpp>
#include <wil/filesystem.h>
#include <wil/resource.h>
#include <wil/result.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

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

namespace z3lx::gfu {
ConfigManager::ConfigManager() noexcept = default;
ConfigManager::~ConfigManager() noexcept = default;

void ConfigManager::FilePath(std::filesystem::path filePath) try {
    this->filePath = std::move(filePath);
    fileHandle = wil::open_or_create_file(
        this->filePath.wstring().c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE
    );
    changeReader = wil::make_folder_change_reader(
        this->filePath.parent_path().wstring().c_str(),
        false, wil::FolderChangeEvents::LastWriteTime,
        [this](const auto event, const auto fileName) {
            OnFolderChange(event, fileName);
        }
    );
    changed.store(true, std::memory_order_relaxed);
} CATCH_THROW_NORMALIZED()

const std::filesystem::path& ConfigManager::FilePath() const noexcept {
    return filePath;
}

Config ConfigManager::Read() const {
    nlohmann::json j {
        nlohmann::json::parse(util::ReadFileA(fileHandle.get()))
    };

    const auto tryGetTo = [&j]<typename ValueT, typename Callable>(
        std::string_view key, ValueT& value, Callable condition) {
        try {
            ValueT parsedValue {};
            j.at(key).get_to(parsedValue);
            if (condition(parsedValue)) {
                value = std::move(parsedValue);
                return;
            }
            throw std::runtime_error {
                "Invalid value for key '" + std::string { key } + "': " +
                "condition is not satisfied"
            };
        } CATCH_LOG()
    };

    const auto isAlwaysValid = [](const auto&) { return true; };
    const auto isValidFov = [](const uint8_t fov) noexcept {
        return fov > 0 && fov < 180;
    };
    const auto isValidFovPresets = [isValidFov](
        std::vector<uint8_t>& fovPresets) noexcept {
        const bool valid = !fovPresets.empty() &&
            std::ranges::all_of(fovPresets, isValidFov);
        if (valid) {
            std::ranges::sort(fovPresets);
            const auto last = std::ranges::unique(fovPresets).begin();
            fovPresets.erase(last, fovPresets.end());
        }
        return valid;
    };
    const auto isValidSmoothing = [](const float smoothing) noexcept {
        return smoothing >= 0.0f && smoothing <= 1.0f;
    };
    const auto isValidKey = [](const uint8_t key) noexcept {
        return key > 0 && key < 255;
    };

    Config config {};

    tryGetTo(ENABLED, config.enabled, isAlwaysValid);
    tryGetTo(FOV, config.fov, isValidFov);
    tryGetTo(FOV_PRESETS, config.fovPresets, isValidFovPresets);
    tryGetTo(SMOOTHING, config.smoothing, isValidSmoothing);
    tryGetTo(ENABLE_KEY, config.enableKey, isValidKey);
    tryGetTo(NEXT_KEY, config.nextKey, isValidKey);
    tryGetTo(PREV_KEY, config.prevKey, isValidKey);
    tryGetTo(DUMP_KEY, config.dumpKey, isValidKey);

    return config;
}

void ConfigManager::Write(const Config& config) const {
    const nlohmann::ordered_json j {
        { ENABLED, config.enabled },
        { FOV, config.fov },
        { FOV_PRESETS, config.fovPresets },
        { SMOOTHING, config.smoothing },
        { ENABLE_KEY, config.enableKey },
        { NEXT_KEY, config.nextKey },
        { PREV_KEY, config.prevKey },
        { DUMP_KEY, config.dumpKey }
    };

    util::WriteFileA(fileHandle.get(), j.dump(4));
}

void ConfigManager::OnFolderChange(
    const wil::FolderChangeEvent event, const PCWSTR filename) noexcept {
    if (event == wil::FolderChangeEvent::Modified &&
        filename == filePath.filename()) {
        changed.store(true, std::memory_order_relaxed);
    }
}

void ConfigManager::Update() noexcept {
    if (changed.load(std::memory_order_relaxed)) {
        changed.store(false, std::memory_order_relaxed);
        Notify(OnConfigChange {});
    }
}
} // namespace z3lx::gfu
