#pragma once

#include "plugin/components/ConfigManager.hpp"
#include "util/win/File.hpp"

#include <wil/filesystem.h>
#include <wil/resource.h>
#include <wil/result.h>

#include <atomic>
#include <filesystem>
#include <utility>

namespace z3lx::gfu {
template <typename ConfigT>
ConfigManager<ConfigT>::ConfigManager() noexcept = default;

template <typename ConfigT>
ConfigManager<ConfigT>::~ConfigManager() noexcept = default;

template <typename ConfigT>
const std::filesystem::path& ConfigManager<ConfigT>::FilePath() const noexcept {
    return filePath;
}

template <typename ConfigT>
void ConfigManager<ConfigT>::FilePath(std::filesystem::path filePath) try {
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

template <typename ConfigT>
const ConfigT& ConfigManager<ConfigT>::Config() const noexcept {
    return config;
}

template <typename ConfigT>
ConfigT& ConfigManager<ConfigT>::Config() noexcept {
    return config;
}

template <typename ConfigT>
void ConfigManager<ConfigT>::Read() {
    util::ReadFile(fileHandle.get(), buffer);
    config.Deserialize(buffer);
}

template <typename ConfigT>
void ConfigManager<ConfigT>::Write() {
    config.Serialize(buffer);
    util::WriteFile(fileHandle.get(), buffer);
}

template <typename ConfigT>
void ConfigManager<ConfigT>::OnFolderChange(
    const wil::FolderChangeEvent event, const PCWSTR filename) noexcept try {
    if (event == wil::FolderChangeEvent::Modified &&
        filename == filePath.filename()) {
        changed.store(true, std::memory_order_relaxed);
    }
} catch (...) {}

template <typename ConfigT>
void ConfigManager<ConfigT>::Update() {
    if (changed.load(std::memory_order_relaxed)) {
        changed.store(false, std::memory_order_relaxed);
        try {
            Read();
        } CATCH_LOG()
        this->Notify(OnConfigChange<ConfigT> { config });
    }
}
} // namespace z3lx::gfu
