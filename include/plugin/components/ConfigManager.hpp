#pragma once

#include "plugin/interfaces/IComponent.hpp"

#include <wil/filesystem.h>
#include <wil/resource.h>

#include <atomic>
#include <cstdint>
#include <filesystem>
#include <vector>

#include <WinUser.h>

namespace z3lx::gfu {
struct OnConfigChange {};

struct Config {
    bool enabled = true;
    uint8_t fov = 75;
    std::vector<uint8_t> fovPresets {
        30, 45, 60, 75, 90, 110
    };
    float smoothing = 0.125;

    uint8_t enableKey = VK_DOWN;
    uint8_t nextKey = VK_RIGHT;
    uint8_t prevKey = VK_LEFT;
};

class ConfigManager final : public IComponent<
    ConfigManager, OnConfigChange> {
public:
    ConfigManager() noexcept;
    ~ConfigManager() noexcept;

    void Update();

    [[nodiscaord]] const std::filesystem::path& FilePath() const noexcept;
    void FilePath(std::filesystem::path filePath);

    [[nodiscard]] Config Read() const;
    void Write(const Config& config) const;

private:
    void OnFolderChange(
        wil::FolderChangeEvent event,
        PCWSTR filename) noexcept;

    std::filesystem::path filePath;
    wil::unique_hfile fileHandle;
    wil::unique_folder_change_reader changeReader;
    std::atomic<bool> changed;
};
} // namespace z3lx::gfu
