#pragma once

#include <filesystem>

#include <wil/result.h>

namespace z3lx::plugin {
using LoggingCallbackFunc = void(*)(const wil::FailureInfo& info) noexcept;
LoggingCallbackFunc GetLoggingCallback(std::filesystem::path logFilePath);
} // namespace z3lx::plugin
