#include "plugin/components/ConfigManager.hpp"
#include "plugin/Config.hpp"
#include "util/win/File.hpp"

#include <wil/filesystem.h>
#include <wil/resource.h>
#include <wil/result.h>

#include <atomic>
#include <filesystem>
#include <utility>

namespace z3lx::gfu {
ConfigManager::ConfigManager() noexcept = default;
ConfigManager::~ConfigManager() noexcept = default;

const std::filesystem::path& ConfigManager::FilePath() const noexcept {
    return filePath;
}

void ConfigManager::FilePath(std::filesystem::path filePath) try {
    this->filePath = std::move(filePath);
    fileHandle = wil::open_or_create_file(
        this->filePath.native().c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE
    );
    changeReader = wil::make_folder_change_reader(
        this->filePath.parent_path().native().c_str(),
        false, wil::FolderChangeEvents::LastWriteTime,
        [this](const auto event, const auto fileName) {
            OnFolderChange(event, fileName);
        }
    );
    changed.store(true, std::memory_order_relaxed);
} CATCH_THROW_NORMALIZED()

const Config& ConfigManager::Config() const noexcept {
    return config;
}

Config& ConfigManager::Config() noexcept {
    return config;
}

void ConfigManager::Read() {
    util::ReadFile(fileHandle.get(), buffer);
    config.Deserialize(buffer);
}

void ConfigManager::Write() {
    config.Serialize(buffer);
    util::WriteFile(fileHandle.get(), buffer);
}

void ConfigManager::OnFolderChange(
    const wil::FolderChangeEvent event, const PCWSTR filename) noexcept try {
    if (event == wil::FolderChangeEvent::Modified &&
        filename == filePath.filename()) {
        changed.store(true, std::memory_order_relaxed);
    }
} catch (...) {}

void ConfigManager::Update() {
    if (changed.load(std::memory_order_relaxed)) {
        changed.store(false, std::memory_order_relaxed);
        try {
            Read();
        } CATCH_LOG()
        Notify(OnConfigChange { config });
    }
}
} // namespace z3lx::gfu
