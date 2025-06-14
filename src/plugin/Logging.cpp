#include "plugin/Logging.hpp"
#include "utils/win/File.hpp"
#include "utils/win/Loader.hpp"
#include "utils/win/String.hpp"

#include <cstdint>
#include <filesystem>
#include <mutex>
#include <string>

#include <wil/filesystem.h>
#include <wil/resource.h>
#include <wil/result.h>

namespace {
wil::unique_hfile fileHandle {};

void LoggingCallback(const wil::FailureInfo& info) noexcept {
    // Prevent recursive logging
    static std::recursive_mutex mutex {};
    static bool isAcquired = false;
    std::lock_guard lock { mutex };
    if (isAcquired) {
        return;
    }
    isAcquired = true;

    // Log message to file
    constexpr size_t messageBufferSize = 2048;
    wchar_t messageBuffer[messageBufferSize] {};
    const HRESULT hr = wil::GetFailureLogString(
        messageBuffer,
        messageBufferSize,
        info
    );

    if (SUCCEEDED(hr)) try {
        // std::fputws(messageBuffer, stderr);
        const std::u8string message = utils::U16ToU8(messageBuffer);
        utils::AppendFileU8(fileHandle.get(), message);
    } catch (...) {}

    isAcquired = false;
}
} // namespace

LoggingCallbackFunc GetLoggingCallback() {
    static std::once_flag flag {};
    std::call_once(flag, []() {
        const std::filesystem::path filePath =
            utils::GetCurrentModuleFilePath().parent_path() / L"log.txt";
        fileHandle = wil::open_or_truncate_existing_file(
            filePath.native().c_str()
        );
    });
    return LoggingCallback;
}
