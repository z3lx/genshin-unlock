#include "loader/Config.hpp"
#include "util/Concepts.hpp"
#include "util/win/Console.hpp"
#include "util/win/File.hpp"
#include "util/win/Loader.hpp"

#include <wil/filesystem.h>
#include <wil/resource.h>
#include <wil/result.h>

#include <chrono>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <Windows.h>
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
} // namespace

int main() try {
    RequestElevation();

    // Read configuration
    z3lx::loader::Config config {};
    std::vector<uint8_t> buffer {};

    constexpr auto configFileName = L"loader_config.json";
    const bool configFileExists = fs::exists(configFileName);
    const wil::unique_hfile configFile =
        wil::open_or_create_file(configFileName);

    if (configFileExists) {
        z3lx::util::ReadFile(configFile.get(), buffer);
    } else {
        config.Serialize(buffer);
        z3lx::util::WriteFile(configFile.get(), buffer);
    }
    config.Deserialize(buffer);

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
    z3lx::util::LoadRemoteLibrary(process.get(), config.dllPaths);
    if (config.suspendLoad) {
        ResumeThread(thread.get());
    }

    std::cout << "Game started successfully." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds { 1 });

    return 0;
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    z3lx::util::Pause();
    return 1;
}
