#include "loader/Config.hpp"
#include "util/Concepts.hpp"
#include "util/win/File.hpp"
#include "util/win/Loader.hpp"

#include <wil/filesystem.h>
#include <wil/resource.h>
#include <wil/result.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <Windows.h>
#include <conio.h>
#include <shellapi.h>

namespace fs = std::filesystem;

namespace {
void RequestElevation() try {
    // Check if process is elevated
    wil::unique_handle token {};
    THROW_IF_WIN32_BOOL_FALSE(OpenProcessToken(
        GetCurrentProcess(),
        TOKEN_QUERY,
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
    const fs::path filePath = z3lx::util::GetCurrentModuleFilePath();
    SHELLEXECUTEINFOW info {
        .cbSize = sizeof(info),
        .hwnd = nullptr,
        .lpVerb = L"runas",
        .lpFile = filePath.c_str(),
        .nShow = SW_NORMAL
    };
    THROW_IF_WIN32_BOOL_FALSE(ShellExecuteExW(
        &info
    ));

    std::exit(0);
} CATCH_THROW_NORMALIZED()

void InjectDlls(
    const HANDLE processHandle,
    const z3lx::util::Container<fs::path> auto& dllPaths) try {
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
    const auto bufferCleanup = wil::scope_exit([=] {
        VirtualFreeEx(
            processHandle,
            buffer,
            0,
            MEM_RELEASE
        );
    });

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
    }
} CATCH_THROW_NORMALIZED()

void Pause() noexcept {
    std::cout << "Press any key to continue..." << std::endl;
    _getch();
}
} // namespace

int main() try {
    RequestElevation();

    // Read configuration
    std::vector<uint8_t> buffer {};
    constexpr auto configFileName = L"loader_config.json";
    const bool configFileExists = fs::exists(configFileName);
    const wil::unique_hfile configFile =
        wil::open_or_create_file(configFileName);

    if (configFileExists) {
        z3lx::util::ReadFile(configFile.get(), buffer);
    } else {
        z3lx::loader::Config {}.Serialize(buffer);
        z3lx::util::WriteFile(configFile.get(), buffer);
    }

    z3lx::loader::Config config =
        z3lx::loader::Config::Deserialize(buffer);

    // Start game process
    STARTUPINFOW si { .cb = sizeof(si) };
    PROCESS_INFORMATION pi {};
    THROW_IF_WIN32_BOOL_FALSE(CreateProcessW(
        config.gamePath.c_str(),
        config.gameArgs.data(),
        nullptr,
        nullptr,
        FALSE,
        config.suspendLoad ? CREATE_SUSPENDED : 0,
        nullptr,
        config.gamePath.parent_path().c_str(),
        &si,
        &pi
    ));
    const wil::unique_handle process { pi.hProcess };
    const wil::unique_handle thread { pi.hThread };

    // Inject DLLs
    InjectDlls(process.get(), config.dllPaths);
    if (config.suspendLoad) {
        ResumeThread(thread.get());
    }

    std::cout << "Game started successfully." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds { 1 });

    return 0;
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    Pause();
    return 1;
}
