#pragma once

#include <Windows.h>

namespace z3lx::util {
template <typename ContiguousContainer>
ContiguousContainer ReadFile(HANDLE fileHandle);

template <typename ContiguousContainer>
void ReadFile(HANDLE fileHandle, ContiguousContainer& buffer);

template <typename ContiguousContainer>
void WriteFile(HANDLE fileHandle, const ContiguousContainer& buffer);

template <typename ContiguousContainer>
void AppendFile(HANDLE fileHandle, const ContiguousContainer& buffer);
} // namespace z3lx::util

#include "util/win/FileInl.hpp"
