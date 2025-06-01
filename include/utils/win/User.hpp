#pragma once

#include <vector>

#include <Windows.h>

namespace utils {
std::vector<HWND> GetCurrentProcessWindows();
std::vector<HWND> GetProcessWindows(DWORD processId);
} // namespace utils
