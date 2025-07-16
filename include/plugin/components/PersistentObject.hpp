#pragma once

#include "plugin/interfaces/IComponent.hpp"

#include <wil/filesystem.h>
#include <wil/resource.h>

#include <atomic>
#include <cstdint>
#include <filesystem>
#include <vector>

namespace z3lx::plugin {
template <typename T>
class PersistentObject final : public IComponent<PersistentObject<T>> {
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

#include "plugin/components/PersistentObjectInl.hpp"
