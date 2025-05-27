#pragma once

#include <filesystem>

namespace utils {
std::filesystem::path GetCurrentModuleFilePath();
std::filesystem::path GetModuleFilePath(const void* address);
} // namespace utils

#include "utils/win/LoaderInl.hpp"
