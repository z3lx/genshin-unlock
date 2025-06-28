#pragma once

#include "plugin/interfaces/IComponent.hpp"

#include <wil/filesystem.h>
#include <wil/resource.h>

#include <atomic>
#include <filesystem>
#include <string>

namespace z3lx::gfu {
template <typename ConfigT>
struct OnConfigChange {
    const ConfigT& config;
};

template <typename ConfigT>
class ConfigManager final : public IComponent<
    ConfigManager<ConfigT>, OnConfigChange<ConfigT>> {
public:
    ConfigManager() noexcept;
    ~ConfigManager() noexcept;

    void Update();

    [[nodiscaord]] const std::filesystem::path& FilePath() const noexcept;
    void FilePath(std::filesystem::path filePath);

    [[nodiscard]] const ConfigT& Config() const noexcept;
    [[nodiscard]] ConfigT& Config() noexcept;

    void Read();
    void Write();

private:
    void OnFolderChange(
        wil::FolderChangeEvent event,
        PCWSTR filename) noexcept;

    ConfigT config;
    std::string buffer;

    std::filesystem::path filePath;
    wil::unique_hfile fileHandle;
    wil::unique_folder_change_reader changeReader;
    std::atomic<bool> changed;
};
} // namespace z3lx::gfu

#include "plugin/components/ConfigManagerInl.hpp"
