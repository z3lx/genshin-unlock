#pragma once

#include "util/win/Loader.hpp"

#include <filesystem>

namespace z3lx::util {
inline std::filesystem::path GetCurrentModuleFilePath() {
    const void* address = reinterpret_cast<void*>(GetCurrentModuleFilePath);
    return GetModuleFilePath(address);
}
} // namespace z3lx::util
