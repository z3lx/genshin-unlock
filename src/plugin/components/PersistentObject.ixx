module;

#define WIN32_LEAN_AND_MEAN
#include <wil/filesystem.h>
#include <wil/resource.h>

#include <Windows.h>

export module plugin:PersistentObject;

import :ComponentBase;

import pwu;
import std;

namespace z3lx::plugin {
template <typename T>
class PersistentObject final : public ComponentBase<PersistentObject<T>> {
public:
    PersistentObject() noexcept;
    ~PersistentObject() noexcept;

    void Update() noexcept;

    [[nodiscard]] const std::filesystem::path& GetFilePath() const noexcept;
    void SetFilePath(std::filesystem::path filePath);

    template <auto Member>
    [[nodiscard]] const decltype(T{}.*Member)& Get() const noexcept;
    template <auto Member>
    void Set(const decltype(T{}.*Member)& value);

    void Read();
    void Write();
    HRESULT TryRead() noexcept;
    HRESULT TryWrite() noexcept;

private:
    void OnFolderChange(wil::FolderChangeEvent event, PCWSTR filename) noexcept;

    T object;
    std::vector<uint8_t> buffer;

    std::filesystem::path filePath;
    wil::unique_hfile fileHandle;
    wil::unique_folder_change_reader changeReader;

    std::atomic<bool> diskChanged;
    bool memoryChanged;
};
} // namespace z3lx::plugin

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
void PersistentObject<T>::SetFilePath(std::filesystem::path filePath) try {
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
} catch (...) {
    pwu::ThrowCaughtTraced();
}

template <typename T>
template <auto Member>
const decltype(T{}.*Member)& PersistentObject<T>::Get() const noexcept {
    return object.*Member;
}

template <typename T>
template <auto Member>
void PersistentObject<T>::Set(const decltype(T{}.*Member)& value) try {
    if (object.*Member != value) {
        object.*Member = value;
        memoryChanged = true;
    }
} catch (...) {
    pwu::ThrowCaughtTraced();
}

template <typename T>
void PersistentObject<T>::Read() try {
    pwu::ReadFile(fileHandle.get(), buffer);
    object.Deserialize(buffer);
} catch (...) {
    pwu::ThrowCaughtTraced();
}

template <typename T>
void PersistentObject<T>::Write() try {
    object.Serialize(buffer);
    pwu::WriteFile(fileHandle.get(), buffer);
} catch (...) {
    pwu::ThrowCaughtTraced();
}

template <typename T>
HRESULT PersistentObject<T>::TryRead() noexcept try {
    Read();
    return S_OK;
} catch (...) {
    return E_FAIL;
}

template <typename T>
HRESULT PersistentObject<T>::TryWrite() noexcept try {
    Write();
    return S_OK;
} catch (...) {
    return E_FAIL;
}

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
