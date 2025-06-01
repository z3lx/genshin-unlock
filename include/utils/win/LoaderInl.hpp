#pragma once

#include "utils/win/Loader.hpp"

#include <filesystem>

namespace utils {
inline std::filesystem::path GetCurrentModuleFilePath() {
    const void* address = reinterpret_cast<void*>(GetCurrentModuleFilePath);
    return GetModuleFilePath(address);
}
} // namespace utils
