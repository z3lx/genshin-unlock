#include "utils/win/File.hpp"

#include <algorithm>
#include <cstdint>
#include <string>
#include <string_view>

#include <wil/result.h>

#include <Windows.h>

namespace {
template <typename CharT>
std::basic_string<CharT> ReadFile(
    HANDLE fileHandle,
    size_t maxChunkSize
);

template <typename CharT>
void WriteFile(
    HANDLE fileHandle,
    std::basic_string_view<CharT> buffer,
    size_t maxChunkSize
);

template <typename CharT>
void AppendFile(
    HANDLE fileHandle,
    std::basic_string_view<CharT> buffer,
    size_t maxChunkSize
);

template <typename CharT>
std::basic_string<CharT> ReadFile(
    const HANDLE fileHandle,
    const size_t maxChunkSize) {
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
            static_cast<DWORD>((std::min)(maxChunkSize, bufferSize - offset));
        THROW_IF_WIN32_BOOL_FALSE(::ReadFile(
            fileHandle, chunkPointer, chunkSize, &bytesTransferred, nullptr
        ));
    }
    return buffer;
}

template <typename CharT>
void WriteFile(
    const HANDLE fileHandle,
    const std::basic_string_view<CharT> buffer,
    const size_t maxChunkSize) {
    THROW_IF_WIN32_BOOL_FALSE(::SetFilePointerEx(
        fileHandle, {}, nullptr, FILE_BEGIN
    ));
    THROW_IF_WIN32_BOOL_FALSE(::SetEndOfFile(fileHandle));
    AppendFile<CharT>(fileHandle, buffer, maxChunkSize);
}

template <typename CharT>
void AppendFile(
    const HANDLE fileHandle,
    const std::basic_string_view<CharT> buffer,
    const size_t maxChunkSize) {
    const size_t bufferSize = buffer.size() * sizeof(CharT);

    DWORD bytesTransferred = 0;
    for (size_t offset = 0; offset < bufferSize; offset += bytesTransferred) {
        const void* chunkPointer =
            reinterpret_cast<const uint8_t*>(buffer.data()) + offset;
        const DWORD chunkSize =
            static_cast<DWORD>((std::min)(maxChunkSize, bufferSize - offset));
        THROW_IF_WIN32_BOOL_FALSE(::WriteFile(
            fileHandle, chunkPointer, chunkSize, &bytesTransferred, nullptr
        ));
    }
}
} // namespace

namespace utils {
std::string ReadFileA(
    const HANDLE fileHandle,
    const size_t chunkSize) {
    return ReadFile<char>(fileHandle, chunkSize);
}

std::wstring ReadFileW(
    const HANDLE fileHandle,
    const size_t chunkSize) {
    return ReadFile<wchar_t>(fileHandle, chunkSize);
}

void WriteFileA(
    const HANDLE fileHandle,
    const std::string_view buffer,
    const size_t chunkSize) {
    WriteFile<char>(fileHandle, buffer, chunkSize);
}

void WriteFileW(
    const HANDLE fileHandle,
    const std::wstring_view buffer,
    const size_t chunkSize) {
    WriteFile<wchar_t>(fileHandle, buffer, chunkSize);
}

void AppendFileA(
    const HANDLE fileHandle,
    const std::string_view buffer,
    const size_t chunkSize) {
    AppendFile<char>(fileHandle, buffer, chunkSize);
}

void AppendFileW(
    const HANDLE fileHandle,
    const std::wstring_view buffer,
    const size_t chunkSize) {
    AppendFile<wchar_t>(fileHandle, buffer, chunkSize);
}
} // namespace utils
