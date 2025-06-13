#pragma once

#include <cstdint>
#include <limits>
#include <string>
#include <string_view>

#include <Windows.h>

namespace utils {
namespace detail {
constexpr DWORD defaultBufferChunkSize = (std::numeric_limits<DWORD>::max)();
} // namespace detail

std::string ReadFileA(
    HANDLE fileHandle,
    size_t bufferChunkSize = detail::defaultBufferChunkSize
);

std::u8string ReadFileU8(
    HANDLE fileHandle,
    size_t bufferChunkSize = detail::defaultBufferChunkSize
);

std::u16string ReadFileU16(
    HANDLE fileHandle,
    size_t bufferChunkSize = detail::defaultBufferChunkSize
);

void WriteFileA(
    HANDLE fileHandle,
    std::string_view buffer,
    size_t bufferChunkSize = detail::defaultBufferChunkSize
);

void WriteFileU8(
    HANDLE fileHandle,
    std::u8string_view buffer,
    size_t bufferChunkSize = detail::defaultBufferChunkSize
);

void WriteFileU16(
    HANDLE fileHandle,
    std::u16string_view buffer,
    size_t bufferChunkSize = detail::defaultBufferChunkSize
);

void AppendFileA(
    HANDLE fileHandle,
    std::string_view buffer,
    size_t bufferChunkSize = detail::defaultBufferChunkSize
);

void AppendFileU8(
    HANDLE fileHandle,
    std::u8string_view buffer,
    size_t bufferChunkSize = detail::defaultBufferChunkSize
);

void AppendFileU16(
    HANDLE fileHandle,
    std::u16string_view buffer,
    size_t bufferChunkSize = detail::defaultBufferChunkSize
);
} // namespace utils
