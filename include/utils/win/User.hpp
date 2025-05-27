#pragma once

#include <vector>

#include <windef.h>

namespace utils {
std::vector<HWND> GetCurrentProcessWindows();
std::vector<HWND> GetProcessWindows(DWORD processId);
} // namespace utils

#include "utils/win/UserInl.hpp"
