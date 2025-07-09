#include "util/win/Loader.hpp"

#include <wil/result.h>

#include <array>
#include <filesystem>
#include <span>
#include <string_view>

#include <Windows.h>

namespace fs = std::filesystem;

namespace z3lx::util {
fs::path GetModuleFilePath(const HMODULE module) {
    std::array<wchar_t, MAX_PATH> buffer {};
    const DWORD stringSize = GetModuleFileNameW(
        module,
        buffer.data(),
        buffer.size()
    );
    THROW_LAST_ERROR_IF(
        stringSize == buffer.size() ||
        stringSize == 0
    );
    return fs::path { buffer.data() };
}

fs::path GetModuleFilePath(const void* address) {
    HMODULE module {};
    THROW_IF_WIN32_BOOL_FALSE(GetModuleHandleExA(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        static_cast<LPCTSTR>(address), &module
    ));
    return GetModuleFilePath(module);
}

std::filesystem::path GetModuleFilePath(const std::string_view moduleName) {
    HMODULE module {};
    THROW_IF_WIN32_BOOL_FALSE(GetModuleHandleExA(
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        moduleName.data(), &module
    ));
    return GetModuleFilePath(module);
}

std::filesystem::path GetModuleFilePath(const std::wstring_view moduleName) {
    HMODULE module {};
    THROW_IF_WIN32_BOOL_FALSE(GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        moduleName.data(), &module
    ));
    return GetModuleFilePath(module);
}

void LoadRemoteLibrary(
    const HANDLE processHandle,
    const std::filesystem::path& libraryPath) {
    const std::span libraryPaths { &libraryPath, 1 };
    LoadRemoteLibrary(processHandle, libraryPaths);
}

} // namespace z3lx::util
