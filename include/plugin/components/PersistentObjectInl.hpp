#pragma once

#include "plugin/components/PersistentObject.hpp"
#include "util/win/File.hpp"

#include <wil/filesystem.h>
#include <wil/resource.h>
#include <wil/result.h>

#include <atomic>
#include <filesystem>
#include <utility>

namespace z3lx::gfu {
template <typename T>
PersistentObject<T>::PersistentObject() noexcept = default;

template <typename T>
PersistentObject<T>::~PersistentObject() noexcept = default;

template <typename T>
const std::filesystem::path& PersistentObject<T>::FilePath() const noexcept {
    return filePath;
}

template <typename T>
void PersistentObject<T>::FilePath(std::filesystem::path filePath) try {
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

template <typename T>
const T& PersistentObject<T>::Object() const noexcept {
    return object;
}

template <typename T>
T& PersistentObject<T>::Object() noexcept {
    return object;
}

template <typename T>
void PersistentObject<T>::Read() {
    util::ReadFile(fileHandle.get(), buffer);
    object.Deserialize(buffer);
}

template <typename T>
void PersistentObject<T>::Write() {
    object.Serialize(buffer);
    util::WriteFile(fileHandle.get(), buffer);
}

template <typename T>
void PersistentObject<T>::OnFolderChange(
    const wil::FolderChangeEvent event, const PCWSTR filename) noexcept try {
    if (event == wil::FolderChangeEvent::Modified &&
        filename == filePath.filename()) {
        changed.store(true, std::memory_order_relaxed);
    }
} catch (...) {}

template <typename T>
void PersistentObject<T>::Update() {
    if (changed.load(std::memory_order_relaxed)) {
        changed.store(false, std::memory_order_relaxed);
        try {
            Read();
        } CATCH_LOG()
        this->Notify(OnPersistentObjectChange<T> { object });
    }
}
} // namespace z3lx::gfu
