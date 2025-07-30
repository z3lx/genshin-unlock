#pragma once

#include "util/Version.hpp"

#include <filesystem>

namespace z3lx::util {
Version GetFileVersion(const std::filesystem::path& filePath);
} // namespace z3lx::util
