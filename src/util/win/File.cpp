#include "util/win/File.hpp"

#include <algorithm>
#include <cstdint>
#include <string>
#include <string_view>

#include <wil/result.h>

#include <Windows.h>

namespace {
template <typename BufferCharT>
std::basic_string<BufferCharT> ReadFile(
    HANDLE fileHandle,
    size_t maxBufferChunkSize
);

template <typename BufferCharT>
void WriteFile(
    HANDLE fileHandle,
    std::basic_string_view<BufferCharT> buffer,
    size_t maxBufferChunkSize
);

template <typename BufferCharT>
void AppendFile(
    HANDLE fileHandle,
    std::basic_string_view<BufferCharT> buffer,
    size_t maxBufferChunkSize
);

template <typename BufferCharT>
std::basic_string<BufferCharT> ReadFile(
    const HANDLE fileHandle,
    const size_t maxBufferChunkSize) {
    THROW_IF_WIN32_BOOL_FALSE(::SetFilePointerEx(
        fileHandle, {}, nullptr, FILE_BEGIN
    ));

    // Calculate buffer size
    LARGE_INTEGER fileSize {};
    THROW_IF_WIN32_BOOL_FALSE(::GetFileSizeEx(fileHandle, &fileSize));
    const auto bufferSize = static_cast<size_t>(fileSize.QuadPart);
    const size_t bufferCharCount =
        (bufferSize + sizeof(BufferCharT) - 1) / sizeof(BufferCharT);
    std::basic_string<BufferCharT> buffer(bufferCharCount, 0);

    // Read file to buffer
    for (size_t bufferSizeOffset = 0; bufferSizeOffset < bufferSize;) {
        void* bufferChunkAddress =
            reinterpret_cast<uint8_t*>(buffer.data()) + bufferSizeOffset;
        const DWORD bufferChunkSize = static_cast<DWORD>((std::min)(
            maxBufferChunkSize,
            bufferSize - bufferSizeOffset
        ));
        DWORD bytesRead = 0;
        THROW_IF_WIN32_BOOL_FALSE(::ReadFile(
            fileHandle,
            bufferChunkAddress,
            bufferChunkSize,
            &bytesRead,
            nullptr
        ));
        bufferSizeOffset += bytesRead;
    }
    return buffer;
}

template <typename BufferCharT>
void WriteFile(
    const HANDLE fileHandle,
    const std::basic_string_view<BufferCharT> buffer,
    const size_t maxBufferChunkSize) {
    THROW_IF_WIN32_BOOL_FALSE(::SetFilePointerEx(
        fileHandle, {}, nullptr, FILE_BEGIN
    ));
    THROW_IF_WIN32_BOOL_FALSE(::SetEndOfFile(fileHandle));
    AppendFile<BufferCharT>(fileHandle, buffer, maxBufferChunkSize);
}

template <typename BufferCharT>
void AppendFile(
    const HANDLE fileHandle,
    const std::basic_string_view<BufferCharT> buffer,
    const size_t maxBufferChunkSize) {
    const size_t bufferSize = buffer.size() * sizeof(BufferCharT);

    for (size_t bufferSizeOffset = 0; bufferSizeOffset < bufferSize;) {
        const void* bufferChunkAddress =
            reinterpret_cast<const uint8_t*>(buffer.data()) + bufferSizeOffset;
        const DWORD bufferChunkSize = static_cast<DWORD>((std::min)(
            maxBufferChunkSize,
            bufferSize - bufferSizeOffset
        ));
        DWORD bytesTransferred = 0;
        THROW_IF_WIN32_BOOL_FALSE(::WriteFile(
            fileHandle,
            bufferChunkAddress,
            bufferChunkSize,
            &bytesTransferred,
            nullptr
        ));
        bufferSizeOffset += bytesTransferred;
    }
}
} // namespace

namespace z3lx::util {
std::string ReadFileA(
    const HANDLE fileHandle,
    const size_t bufferChunkSize) {
    return ReadFile<char>(fileHandle, bufferChunkSize);
}

std::u8string ReadFileU8(
    const HANDLE fileHandle,
    const size_t bufferChunkSize) {
    return ReadFile<char8_t>(fileHandle, bufferChunkSize);
}

std::u16string ReadFileU16(
    const HANDLE fileHandle,
    const size_t bufferChunkSize) {
    return ReadFile<char16_t>(fileHandle, bufferChunkSize);
}

void WriteFileA(
    const HANDLE fileHandle,
    const std::string_view buffer,
    const size_t bufferChunkSize) {
    WriteFile<char>(fileHandle, buffer, bufferChunkSize);
}

void WriteFileU8(
    const HANDLE fileHandle,
    const std::u8string_view buffer,
    const size_t bufferChunkSize) {
    WriteFile<char8_t>(fileHandle, buffer, bufferChunkSize);
}

void WriteFileU16(
    const HANDLE fileHandle,
    const std::u16string_view buffer,
    const size_t bufferChunkSize) {
    WriteFile<char16_t>(fileHandle, buffer, bufferChunkSize);
}

void AppendFileA(
    const HANDLE fileHandle,
    const std::string_view buffer,
    const size_t bufferChunkSize) {
    AppendFile<char>(fileHandle, buffer, bufferChunkSize);
}

void AppendFileU8(
    const HANDLE fileHandle,
    const std::u8string_view buffer,
    const size_t bufferChunkSize) {
    AppendFile<char8_t>(fileHandle, buffer, bufferChunkSize);
}

void AppendFileU16(
    const HANDLE fileHandle,
    const std::u16string_view buffer,
    const size_t bufferChunkSize) {
    AppendFile<char16_t>(fileHandle, buffer, bufferChunkSize);
}
} // namespace z3lx::util
