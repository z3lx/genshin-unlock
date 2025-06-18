#pragma once

#include <wil/result.h>

namespace z3lx::gfu {
using LoggingCallbackFunc = void(*)(const wil::FailureInfo& info) noexcept;
LoggingCallbackFunc GetLoggingCallback();
} // namespace z3lx::gfu
