#include "util/Version.hpp"
#include "util/win/Version.hpp"

#include <wil/result.h>

#include <cstdint>
#include <filesystem>
#include <vector>

#include <Windows.h>

namespace z3lx::util {
Version GetFileVersion(const std::filesystem::path& filePath) {
    const DWORD infoSize = GetFileVersionInfoSizeW(
        filePath.c_str(),
        nullptr
    );
    THROW_LAST_ERROR_IF(infoSize == 0);

    std::vector<uint8_t> infoBuffer(infoSize, 0);
    THROW_IF_WIN32_BOOL_FALSE(GetFileVersionInfoW(
        filePath.c_str(),
        0,
        infoBuffer.size(),
        infoBuffer.data()
    ));

    PVOID queryBuffer = nullptr;
    UINT queryBufferSize = 0;
    THROW_IF_WIN32_BOOL_FALSE(VerQueryValueW(
        infoBuffer.data(),
        L"\\",
        &queryBuffer,
        &queryBufferSize
    ));

    const auto& info = *static_cast<VS_FIXEDFILEINFO*>(queryBuffer);
    THROW_WIN32_IF(ERROR_INVALID_DATA, info.dwSignature != 0xFEEF04BD);

    return Version {
        HIWORD(info.dwFileVersionMS),
        LOWORD(info.dwFileVersionMS),
        HIWORD(info.dwFileVersionLS),
        LOWORD(info.dwFileVersionLS)
    };
}
} // namespace z3lx::util
