#pragma once

#include <atomic>
#include <cstdint>
#include <filesystem>
#include <string>
#include <thread>
#include <vector>

#include <Windows.h>

enum class Action : uint8_t {
    FileAdded = FILE_ACTION_ADDED,
    FileRemoved = FILE_ACTION_REMOVED,
    FileModified = FILE_ACTION_MODIFIED,
    FileRenamedOldName = FILE_ACTION_RENAMED_OLD_NAME,
    FileRenamedNewName = FILE_ACTION_RENAMED_NEW_NAME,
};

template <typename Callable>
concept IsFileWatcherCallback = std::is_nothrow_invocable_r_v<
    void, Callable, std::wstring_view, Action>;

class FileWatcher {
public:
    template <typename Callable> requires IsFileWatcherCallback<Callable>
    FileWatcher(const std::filesystem::path& path, Callable callback);
    ~FileWatcher() noexcept;

private:
    template <typename Callable>
    void WatchFile(Callable callback) noexcept;

    HANDLE directoryHandle;
    std::atomic<bool> flag;
    std::thread thread;
    std::vector<uint8_t> buffer;
};

#include "utils/FileWatcherInl.hpp"
