#pragma once

#include "plugin/components/PersistentObject.hpp"
#include "util/win/File.hpp"

#include <wil/filesystem.h>
#include <wil/resource.h>
#include <wil/result.h>

#include <atomic>
#include <filesystem>
#include <utility>

namespace z3lx::plugin {
template <typename T>
PersistentObject<T>::PersistentObject() noexcept = default;

template <typename T>
PersistentObject<T>::~PersistentObject() noexcept = default;

template <typename T>
const std::filesystem::path& PersistentObject<T>::GetFilePath() const noexcept {
    return filePath;
}

template <typename T>
void PersistentObject<T>::SetFilePath(std::filesystem::path filePath) {
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
    diskChanged.store(true, std::memory_order_relaxed);
}

template <typename T>
template <typename U>
const U& PersistentObject<T>::Get(U T::*member) const noexcept {
    return object.*member;
}

template <typename T>
template <typename U>
void PersistentObject<T>::Set(U T::*member, const U& value) {
    if (object.*member != value) {
        object.*member = value;
        memoryChanged = true;
    }
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
HRESULT PersistentObject<T>::TryRead() noexcept try {
    Read();
    return S_OK;
} CATCH_RETURN()

template <typename T>
HRESULT PersistentObject<T>::TryWrite() noexcept try {
    Write();
    return S_OK;
} CATCH_RETURN()

template <typename T>
void PersistentObject<T>::OnFolderChange(
    const wil::FolderChangeEvent event, const PCWSTR filename) noexcept try {
    if (event == wil::FolderChangeEvent::Modified &&
        filename == filePath.filename()) {
        diskChanged.store(true, std::memory_order_relaxed);
    }
} catch (...) {}

template <typename T>
void PersistentObject<T>::Update() noexcept {
    if (diskChanged.load(std::memory_order_relaxed)) {
        diskChanged.store(false, std::memory_order_relaxed);
        if (FAILED(TryRead())) {
            TryWrite();
        }
    } else if (memoryChanged) {
        memoryChanged = false;
        TryWrite();
    }
}
} // namespace z3lx::plugin
