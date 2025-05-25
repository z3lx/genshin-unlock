#pragma once

#include "utils/win/Filesystem.hpp"

#include <algorithm>
#include <cstdint>
#include <string>
#include <string_view>

#include <wil/result.h>

#include <Windows.h>

namespace utils::detail {
template <typename CharT, size_t ChunkSize>
std::basic_string<CharT> ReadFile(HANDLE fileHandle);

template <typename CharT, size_t ChunkSize>
void WriteFile(HANDLE fileHandle, std::basic_string_view<CharT> buffer);

template <typename CharT, size_t ChunkSize>
void AppendFile(HANDLE fileHandle, std::basic_string_view<CharT> buffer);
} // namespace utils::detail

template <typename CharT, size_t ChunkSize>
std::basic_string<CharT> utils::detail::ReadFile(const HANDLE fileHandle) {
    THROW_IF_WIN32_BOOL_FALSE(::SetFilePointerEx(
        fileHandle, {}, nullptr, FILE_BEGIN
    ));

    LARGE_INTEGER fileSize {};
    THROW_IF_WIN32_BOOL_FALSE(::GetFileSizeEx(fileHandle, &fileSize));
    const auto bufferSize = static_cast<size_t>(fileSize.QuadPart);
    std::basic_string<CharT> buffer(bufferSize / sizeof(CharT), 0);

    DWORD bytesTransferred = 0;
    for (size_t offset = 0; offset < bufferSize; offset += bytesTransferred) {
        void* chunkPointer =
            reinterpret_cast<uint8_t*>(buffer.data()) + offset;
        const DWORD chunkSize =
            static_cast<DWORD>((std::min)(ChunkSize, bufferSize - offset));
        THROW_IF_WIN32_BOOL_FALSE(::ReadFile(
            fileHandle, chunkPointer, chunkSize, &bytesTransferred, nullptr
        ));
    }
    return buffer;
}

template <typename CharT, size_t ChunkSize>
void utils::detail::WriteFile(
    const HANDLE fileHandle, const std::basic_string_view<CharT> buffer) {
    THROW_IF_WIN32_BOOL_FALSE(::SetFilePointerEx(
        fileHandle, {}, nullptr, FILE_BEGIN
    ));
    THROW_IF_WIN32_BOOL_FALSE(::SetEndOfFile(fileHandle));
    AppendFile<CharT, ChunkSize>(fileHandle, buffer);
}

template <typename CharT, size_t ChunkSize>
void utils::detail::AppendFile(
    const HANDLE fileHandle, const std::basic_string_view<CharT> buffer) {
    const size_t bufferSize = buffer.size() * sizeof(CharT);

    DWORD bytesTransferred = 0;
    for (size_t offset = 0; offset < bufferSize; offset += bytesTransferred) {
        const void* chunkPointer =
            reinterpret_cast<const uint8_t*>(buffer.data()) + offset;
        const DWORD chunkSize =
            static_cast<DWORD>((std::min)(ChunkSize, bufferSize - offset));
        THROW_IF_WIN32_BOOL_FALSE(::WriteFile(
            fileHandle, chunkPointer, chunkSize, &bytesTransferred, nullptr
        ));
    }
}

template <size_t ChunkSize>
std::string utils::ReadFileA(const HANDLE fileHandle) {
    return detail::ReadFile<char, ChunkSize>(fileHandle);
}

template <size_t ChunkSize>
std::wstring utils::ReadFileW(const HANDLE fileHandle) {
    return detail::ReadFile<wchar_t, ChunkSize>(fileHandle);
}

template <size_t ChunkSize>
void utils::WriteFileA(
    const HANDLE fileHandle, const std::string_view buffer) {
    detail::WriteFile<char, ChunkSize>(fileHandle, buffer);
}

template <size_t ChunkSize>
void utils::WriteFileW(
    const HANDLE fileHandle, const std::wstring_view buffer) {
    detail::WriteFile<wchar_t, ChunkSize>(fileHandle, buffer);
}

template <size_t ChunkSize>
void utils::AppendFileA(
    const HANDLE fileHandle, const std::string_view buffer) {
    detail::AppendFile<char, ChunkSize>(fileHandle, buffer);
}

template <size_t ChunkSize>
void utils::AppendFileW(
    const HANDLE fileHandle, const std::wstring_view buffer) {
    detail::AppendFile<wchar_t, ChunkSize>(fileHandle, buffer);
}
