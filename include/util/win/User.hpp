#pragma once

#include <vector>

#include <Windows.h>

namespace z3lx::util {
std::vector<HWND> GetCurrentProcessWindows();
std::vector<HWND> GetProcessWindows(DWORD processId);
} // namespace z3lx::util
