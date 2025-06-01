#pragma once

#include <cstdint>
#include <limits>
#include <string>
#include <string_view>

#include <Windows.h>

namespace utils {
namespace detail {
constexpr DWORD ReadWriteDefaultChunkSize =
    (std::numeric_limits<DWORD>::max)();
} // namespace detail

std::string ReadFileA(
    HANDLE fileHandle,
    size_t chunkSize = detail::ReadWriteDefaultChunkSize
);

std::wstring ReadFileW(
    HANDLE fileHandle,
    size_t chunkSize = detail::ReadWriteDefaultChunkSize
);

void WriteFileA(
    HANDLE fileHandle,
    std::string_view buffer,
    size_t chunkSize = detail::ReadWriteDefaultChunkSize
);

void WriteFileW(
    HANDLE fileHandle,
    std::wstring_view buffer,
    size_t chunkSize = detail::ReadWriteDefaultChunkSize
);

void AppendFileA(
    HANDLE fileHandle,
    std::string_view buffer,
    size_t chunkSize = detail::ReadWriteDefaultChunkSize
);

void AppendFileW(
    HANDLE fileHandle,
    std::wstring_view buffer,
    size_t chunkSize = detail::ReadWriteDefaultChunkSize
);
} // namespace utils
