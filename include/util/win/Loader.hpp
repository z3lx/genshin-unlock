#pragma once

#include <filesystem>

namespace z3lx::util {
std::filesystem::path GetCurrentModuleFilePath();
std::filesystem::path GetModuleFilePath(const void* address);
} // namespace z3lx::util

#include "util/win/LoaderInl.hpp"
