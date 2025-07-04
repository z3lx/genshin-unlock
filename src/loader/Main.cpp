#include "util/win/Loader.hpp"

#include <wil/resource.h>
#include <wil/result.h>

#include <algorithm>
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <Windows.h>
#include <shellapi.h>

namespace fs = std::filesystem;

void RequestElevation() {
    // Check if process is elevated
    const auto currentProcess = GetCurrentProcess();
    constexpr auto desiredAccess = TOKEN_QUERY;
    wil::unique_handle token {};
    THROW_IF_WIN32_BOOL_FALSE(OpenProcessToken(
        currentProcess,
        desiredAccess,
        token.put()
    ));

    TOKEN_ELEVATION elevation {};
    DWORD returnElevationSize {};
    THROW_IF_WIN32_BOOL_FALSE(GetTokenInformation(
        token.get(),
        TokenElevation,
        &elevation,
        sizeof(elevation),
        &returnElevationSize
    ));

    if (elevation.TokenIsElevated) {
        return;
    }

    // Restart process with elevated privileges
    const std::filesystem::path filePath =
        z3lx::util::GetCurrentModuleFilePath();
    SHELLEXECUTEINFOW info {
        .cbSize = sizeof(info),
        .hwnd = nullptr,
        .lpVerb = L"runas",
        .lpFile = filePath.native().c_str(),
        .nShow = SW_NORMAL
    };
    THROW_IF_WIN32_BOOL_FALSE(ShellExecuteExW(
        &info
    ));

    std::exit(0);
}

void InjectDlls(
    const HANDLE processHandle,
    const std::vector<std::filesystem::path>& dllPaths) {
    if (dllPaths.empty()) {
        return;
    }

    // Adjust privileges
    LUID luid {};
    THROW_IF_WIN32_BOOL_FALSE(LookupPrivilegeValueA(
        nullptr,
        "SeDebugPrivilege", // SE_DEBUG_NAME 20L
        &luid
    ));

    wil::unique_handle token {};
    THROW_IF_WIN32_BOOL_FALSE(OpenProcessToken(
        GetCurrentProcess(),
        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
        token.put()
    ));

    TOKEN_PRIVILEGES newPrivileges {
        .PrivilegeCount = 1,
        .Privileges = {{
            .Luid = luid,
            .Attributes = SE_PRIVILEGE_ENABLED
        }}
    };
    THROW_IF_WIN32_BOOL_FALSE(AdjustTokenPrivileges(
        token.get(),
        FALSE,
        &newPrivileges,
        sizeof(newPrivileges),
        nullptr,
        nullptr
    ));

    // Get LoadLibraryW
    const HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
    THROW_LAST_ERROR_IF_NULL(kernel32);
    const FARPROC loadLibraryW = GetProcAddress(kernel32, "LoadLibraryW");
    THROW_LAST_ERROR_IF_NULL(loadLibraryW);

    // Calculate buffer size
    const fs::path& longestFilePath = *std::ranges::max_element(
        dllPaths,
        [](const fs::path& a, const fs::path& b) {
            return a.native().size() < b.native().size();
        }
    );
    const size_t bufferSize =
        (longestFilePath.native().size() + 1) * sizeof(wchar_t);

    // Allocate buffer
    const LPVOID buffer = VirtualAllocEx(
        processHandle,
        nullptr,
        bufferSize,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );
    THROW_LAST_ERROR_IF_NULL(buffer);
    const auto cleanup = wil::scope_exit([=] {
        VirtualFreeEx(
            processHandle,
            buffer,
            0,
            MEM_RELEASE
        );
    });

    // const std::vector<char> emptyBuffer(bufferSize, 0);

    for (const fs::path& dllPath : dllPaths) {
        // Write dll path to process
        THROW_IF_WIN32_BOOL_FALSE(WriteProcessMemory(
            processHandle,
            buffer,
            dllPath.c_str(),
            (dllPath.native().size() + 1) * sizeof(wchar_t),
            nullptr
        ));

        // Create thread to load dll
        const wil::unique_handle thread { CreateRemoteThread(
            processHandle,
            nullptr,
            0,
            reinterpret_cast<LPTHREAD_START_ROUTINE>(loadLibraryW),
            buffer,
            0,
            nullptr
        ) };
        THROW_LAST_ERROR_IF_NULL(thread.get());
        WaitForSingleObject(thread.get(), INFINITE);

    //     // Cleanup
    //     WriteProcessMemory(
    //         processHandle, buffer, emptyBuffer.data(), dllPathBufferSize,
    //         &bytesWritten
    //     );
    }
}

int main() try {
    RequestElevation();

    const fs::path gamePath = R"(C:\Program Files\HoYoPlay\games\Genshin Impact game\GenshinImpact.exe)";
    std::wstring gameArgs = gamePath.wstring() + L" -popupwindow -screen-fullscreen 0";
    const fs::path gameDirectory = gamePath.parent_path();
    const std::vector dllPaths { fs::absolute("plugin.dll") };
    constexpr bool suspendLoad = false;

    STARTUPINFOW si { .cb = sizeof(si) };
    PROCESS_INFORMATION pi {};
    THROW_IF_WIN32_BOOL_FALSE(CreateProcessW(
        gamePath.c_str(),
        gameArgs.data(),
        nullptr,
        nullptr,
        FALSE,
        suspendLoad ? CREATE_SUSPENDED : 0,
        nullptr,
        gameDirectory.c_str(),
        &si,
        &pi
    ));

    InjectDlls(pi.hProcess, dllPaths);

    if (suspendLoad) {
        ResumeThread(pi.hThread);
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    return 0;
} catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
}
