#include "util/win/Loader.hpp"

#include <filesystem>

#include <wil/result.h>

#include <Windows.h>

namespace z3lx::util {
std::filesystem::path GetModuleFilePath(const void* address) {
    HMODULE module {};
    THROW_IF_WIN32_BOOL_FALSE(::GetModuleHandleExA(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        static_cast<LPCTSTR>(address), &module
    ));

    constexpr size_t bufferSize = MAX_PATH;
    wchar_t buffer[bufferSize];
    const DWORD stringSize = ::GetModuleFileNameW(module, buffer, bufferSize);
    THROW_LAST_ERROR_IF(stringSize == bufferSize || stringSize == 0);
    return std::filesystem::path { buffer };
}
} // namespace z3lx::util
