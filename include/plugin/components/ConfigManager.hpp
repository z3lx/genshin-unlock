#pragma once

#include "plugin/Config.hpp"
#include "plugin/interfaces/IComponent.hpp"

#include <wil/filesystem.h>
#include <wil/resource.h>

#include <atomic>
#include <filesystem>
#include <string>

namespace z3lx::gfu {
struct OnConfigChange {
    const Config& config;
};

class ConfigManager final : public IComponent<
    ConfigManager, OnConfigChange> {
public:
    ConfigManager() noexcept;
    ~ConfigManager() noexcept;

    void Update();

    [[nodiscaord]] const std::filesystem::path& FilePath() const noexcept;
    void FilePath(std::filesystem::path filePath);

    [[nodiscard]] const gfu::Config& Config() const noexcept;
    [[nodiscard]] gfu::Config& Config() noexcept;

    void Read();
    void Write();

private:
    void OnFolderChange(
        wil::FolderChangeEvent event,
        PCWSTR filename) noexcept;

    gfu::Config config;
    std::string buffer;

    std::filesystem::path filePath;
    wil::unique_hfile fileHandle;
    wil::unique_folder_change_reader changeReader;
    std::atomic<bool> changed;
};
} // namespace z3lx::gfu
