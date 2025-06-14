#pragma once

#include <wil/result.h>

using LoggingCallbackFunc = void(*)(const wil::FailureInfo& info) noexcept;
LoggingCallbackFunc GetLoggingCallback();
