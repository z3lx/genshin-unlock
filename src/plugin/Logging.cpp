#include "plugin/Logging.hpp"
#include "util/win/File.hpp"
#include "util/win/Loader.hpp"
#include "util/win/String.hpp"

#include <wil/filesystem.h>
#include <wil/resource.h>
#include <wil/result.h>

#include <cstdint>
#include <filesystem>
#include <mutex>
#include <string>

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
        const std::u8string message = z3lx::util::U16ToU8(messageBuffer);
        z3lx::util::AppendFileU8(fileHandle.get(), message);
    } catch (...) {}

    isAcquired = false;
}
} // namespace

namespace z3lx::gfu {
LoggingCallbackFunc GetLoggingCallback() {
    static std::once_flag flag {};
    std::call_once(flag, []() {
        const std::filesystem::path currentPath =
            util::GetCurrentModuleFilePath().parent_path();
        fileHandle = wil::open_or_truncate_existing_file(
            (currentPath / "log.txt").native().c_str()
        );
    });
    return LoggingCallback;
}
} // namespace z3lx::gfu
