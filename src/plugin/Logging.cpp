#include "plugin/Logging.hpp"
#include "util/win/File.hpp"
#include "util/win/String.hpp"

#include <wil/filesystem.h>
#include <wil/resource.h>
#include <wil/result.h>

#include <array>
#include <filesystem>
#include <mutex>
#include <string>
#include <string_view>

namespace {
std::filesystem::path logFilePath {};

void LoggingCallback(const wil::FailureInfo& info) noexcept {
    // Prevent recursive logging
    static std::recursive_mutex mutex {};
    static bool isAcquired = false;
    std::lock_guard lock { mutex };
    if (isAcquired) {
        return;
    }
    isAcquired = true;

    try {
        // Open or create file
        static wil::unique_hfile fileHandle {};
        if (!fileHandle.is_valid()) {
            fileHandle = wil::open_or_truncate_existing_file(
                logFilePath.native().c_str()
            );
        }

        // Log message to file
        static std::array<wchar_t, 2048> messageBuffer {};
        const HRESULT hr = wil::GetFailureLogString(
            messageBuffer.data(),
            messageBuffer.size(),
            info
        );

        if (SUCCEEDED(hr)) {
            // std::fputws(messageBuffer, stderr);
            const std::wstring_view message { messageBuffer.data() };
            static std::u8string converted {};
            z3lx::util::U16ToU8(message, converted);
            z3lx::util::AppendFile(fileHandle.get(), converted);
        }
    } catch (...) {}

    isAcquired = false;
}
} // namespace

namespace z3lx::plugin {
LoggingCallbackFunc GetLoggingCallback(std::filesystem::path logFilePath) {
    ::logFilePath = std::move(logFilePath);
    return LoggingCallback;
}
} // namespace z3lx::plugin
