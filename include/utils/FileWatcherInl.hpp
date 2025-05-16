#pragma once

#include "utils/FileWatcher.hpp"

#include <filesystem>
#include <string>
#include <thread>

#include <Windows.h>

enum class FileWatcher::State : uint8_t {
    NonCancellable, Cancellable, Cancelling
};

template <typename Callable> requires IsFileWatcherCallback<Callable>
FileWatcher::FileWatcher(const std::filesystem::path& path, Callable callback)
    : directoryHandle { nullptr }
    , overlapped {}
    , state { State::NonCancellable }
    , buffer(16 * 1024) /* 16 KiB */ {
    directoryHandle = CreateFileW(
        path.wstring().c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        nullptr
    );
    thread = std::thread { &FileWatcher::WatchFile<Callable>, this, callback };
}

inline FileWatcher::~FileWatcher() noexcept {
    if (const auto previous = state.exchange(State::Cancelling);
        previous == State::Cancellable) {
        CancelIoEx(directoryHandle, &overlapped);
    }
    thread.join();
    CloseHandle(directoryHandle);
}

template <typename Callable>
void FileWatcher::WatchFile(Callable callback) noexcept {
    while (true) {
        BOOL success {};
        success = ReadDirectoryChangesW(
            directoryHandle,
            static_cast<LPVOID>(buffer.data()),
            static_cast<DWORD>(buffer.size()),
            TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME |
            FILE_NOTIFY_CHANGE_DIR_NAME |
            FILE_NOTIFY_CHANGE_LAST_WRITE,
            nullptr,
            &overlapped,
            nullptr
        );

        if (!success) {
            switch (GetLastError()) {
            case ERROR_IO_PENDING:
                break;
            default:
                // TODO: ERROR HANDLING
                break;
            }
        }

        if (const auto previous = state.exchange(State::Cancellable);
            previous == State::Cancelling) {
            state.store(State::Cancelling, std::memory_order_relaxed);
            CancelIoEx(directoryHandle, &overlapped);
        }

        DWORD bytesReturned {};
        success = GetOverlappedResult(
            directoryHandle,
            &overlapped,
            &bytesReturned,
            TRUE
        );

        if (!success) {
            switch (GetLastError()) {
            case ERROR_OPERATION_ABORTED:
                return;
            default:
                // TODO: ERROR HANDLING
                break;
            }
        }

        if (const auto previous = state.exchange(State::NonCancellable);
            previous == State::Cancelling) {
            return;
        }

        DWORD offset {};
        while (offset < bytesReturned) {
            const auto* info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(
                buffer.data() + offset);
            std::wstring_view fileName {
                info->FileName, info->FileNameLength / sizeof(wchar_t) };
            const auto action = static_cast<Action>(info->Action);
            callback(fileName, action);

            if (info->NextEntryOffset == 0) {
                break;
            }
            offset += info->NextEntryOffset;
        }
    }
}
