#pragma once

#include <cstdint>
#include <limits>
#include <string>
#include <string_view>

#include <minwindef.h>

namespace utils {
namespace detail {
constexpr auto ReadWriteDefaultChunkSize = (std::numeric_limits<DWORD>::max)();
} // namespace detail

template <size_t ChunkSize = detail::ReadWriteDefaultChunkSize>
std::string ReadFileA(HANDLE fileHandle);

template <size_t ChunkSize = detail::ReadWriteDefaultChunkSize>
std::wstring ReadFileW(HANDLE fileHandle);

template <size_t ChunkSize = detail::ReadWriteDefaultChunkSize>
void WriteFileA(HANDLE fileHandle, std::string_view buffer);

template <size_t ChunkSize = detail::ReadWriteDefaultChunkSize>
void WriteFileW(HANDLE fileHandle, std::wstring_view buffer);

template <size_t ChunkSize = detail::ReadWriteDefaultChunkSize>
void AppendFileA(HANDLE fileHandle, std::string_view buffer);

template <size_t ChunkSize = detail::ReadWriteDefaultChunkSize>
void AppendFileW(HANDLE fileHandle, std::wstring_view buffer);
} // namespace utils

#include "utils/win/FileInl.hpp"
