#pragma once

#include "plugin/interfaces/IComponent.hpp"

#include <wil/filesystem.h>
#include <wil/resource.h>

#include <atomic>
#include <cstdint>
#include <filesystem>
#include <vector>

#undef GetObject

namespace z3lx::plugin {
template <typename T>
struct OnPersistentObjectChange {
    const T& object;
};

template <typename T>
class PersistentObject final : public IComponent<
    PersistentObject<T>, OnPersistentObjectChange<T>> {
public:
    PersistentObject() noexcept;
    ~PersistentObject() noexcept;

    void Update();

    [[nodiscaord]] const std::filesystem::path& GetFilePath() const noexcept;
    void SetFilePath(std::filesystem::path filePath);

    [[nodiscard]] const T& GetObject() const noexcept;
    [[nodiscard]] T& GetObject() noexcept;

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
    std::atomic<bool> changed;
};
} // namespace z3lx::gfu

#include "plugin/components/PersistentObjectInl.hpp"
