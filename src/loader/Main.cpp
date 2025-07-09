#include "loader/Config.hpp"
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
#include <thread>
#include <vector>

#include <Windows.h>

int main() try {
    // Read configuration
    z3lx::loader::Config config {};
    std::vector<uint8_t> buffer {};

    constexpr auto configFileName = L"loader_config.json";
    const bool configFileExists = std::filesystem::exists(configFileName);
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
