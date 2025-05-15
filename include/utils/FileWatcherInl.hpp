#pragma once

#include "utils/FileWatcher.hpp"

#include <filesystem>
#include <string>
#include <thread>

#include <Windows.h>

template <typename Callable> requires IsFileWatcherCallback<Callable>
FileWatcher::FileWatcher(const std::filesystem::path& path, Callable callback)
    : directoryHandle { nullptr }
    , flag { true }
    , buffer(16 * 1024) /* 16 KiB */ {
    directoryHandle = CreateFileW(
        path.wstring().c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        nullptr
    );
    thread = std::thread { &FileWatcher::WatchFile<Callable>, this, callback };
}

inline FileWatcher::~FileWatcher() noexcept {
    // TODO: FIX RACE CONDITION (with atomic state CAS)
    flag.store(false);
    CancelSynchronousIo(thread.native_handle());
    CloseHandle(directoryHandle);
    thread.join();
}

template <typename Callable>
void FileWatcher::WatchFile(Callable callback) noexcept {
    while (flag.load()) {
        DWORD bytesReturned {};
        const auto status = ReadDirectoryChangesW(
            directoryHandle,
            static_cast<LPVOID>(buffer.data()),
            static_cast<DWORD>(buffer.size()),
            TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME |
            FILE_NOTIFY_CHANGE_DIR_NAME |
            FILE_NOTIFY_CHANGE_LAST_WRITE,
            &bytesReturned,
            nullptr,
            nullptr
        );

        if (!status) {
            if (GetLastError() == ERROR_OPERATION_ABORTED) {
                break;
            }
            // TODO: HANDLE ERROR
            continue;
        }

        DWORD offset = 0;
        while (offset < bytesReturned) {
            const auto* info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(
                buffer.data() + offset);
            const auto* name = info->FileName;
            const auto size = info->FileNameLength / sizeof(wchar_t);

            std::wstring_view fileName { name, size };
            const auto action = static_cast<Action>(info->Action);
            callback(fileName, action);

            if (info->NextEntryOffset == 0) {
                break;
            }
            offset += info->NextEntryOffset;
        }
    }
}
