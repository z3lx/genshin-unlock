#include "loader/Config.hpp"
#include "loader/Version.hpp"
#include "util/win/Dialogue.hpp"
#include "util/win/File.hpp"
#include "util/win/Loader.hpp"
#include "util/win/Shell.hpp"
#include "util/win/Version.hpp"

#include <wil/filesystem.h>
#include <wil/resource.h>
#include <wil/result.h>

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <format>
#include <iostream>
#include <print>
#include <string>
#include <thread>
#include <vector>

#include <Windows.h>

namespace z {
using namespace z3lx::loader;
using namespace z3lx::util;
} // namespace z

namespace {
z::Config ReadConfig() {
    std::println(std::cout, "Reading configuration...");

    z::Config config {};
    std::vector<uint8_t> buffer {};
    constexpr auto configFileName = L"loader_config.json";
    const bool configFileExists = std::filesystem::exists(configFileName);
    const wil::unique_hfile configFile =
        wil::open_or_create_file(configFileName);

    if (configFileExists) {
        z::ReadFile(configFile.get(), buffer);
    } else {
        config.Serialize(buffer);
        z::WriteFile(configFile.get(), buffer);
    }
    config.Deserialize(buffer);

    return config;
}

void CheckUpdates(const z::Config& config) {
    if (!config.checkUpdates) {
        return;
    }

    std::println(std::cout, "Checking for updates...");
    const z::Version currentVersion = z::GetCurrentVersion();
    const z::Version latestVersion = z::GetLatestVersion();
    if (latestVersion > currentVersion) {
        const z::MessageBoxResult result = z::ShowMessageBox(
            "Loader",
            std::format(
                "An updated version of the mod is available.\n"
                "Installed version: {}\n"
                "Available version: {}\n"
                "Open the download page?",
                currentVersion.ToString(),
                latestVersion.ToString()
            ),
            z::MessageBoxIcon::Information,
            z::MessageBoxButton::YesNo
        );
        if (result == z::MessageBoxResult::Yes) {
            z::OpenUrl("https://github.com/z3lx/genshin-fov-unlock/releases/latest");
            std::exit(0);
        }
    }
}

void StartGame(const z::Config& config) {
    std::println(std::cout, "Starting game process...");

    std::wstring args = [&config] {
        if (!config.overrideArgs) {
            return std::wstring {};
        }

        const wchar_t* modeArgs = [](const z::DisplayMode mode) {
            switch (mode) {
            case z::DisplayMode::Windowed:
                return L"-screen-fullscreen 0";
            case z::DisplayMode::Fullscreen:
                return L"-screen-fullscreen 1 -window-mode exclusive";
            case z::DisplayMode::Borderless:
                return L"-popupwindow -screen-fullscreen 0";
            default:
                return L"";
            }
        }(config.displayMode);

        const wchar_t* mobileArgs = config.mobilePlatform
            ? L"use_mobile_platform -is_cloud 1 "
                "-platform_type CLOUD_THIRD_PARTY_MOBILE"
            : L"";

        return std::format(
            L"-monitor {} {} -screen-width {} -screen-height {} {} {} ",
            config.monitorIndex,
            modeArgs,
            config.screenWidth,
            config.screenHeight,
            mobileArgs,
            config.additionalArgs
        );
    }();

    STARTUPINFOW si { .cb = sizeof(si) };
    PROCESS_INFORMATION pi {};
    THROW_IF_WIN32_BOOL_FALSE(CreateProcessW(
        config.gamePath.c_str(),
        args.data(),
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

    z::LoadRemoteLibrary(process.get(), config.dllPaths);
    if (config.suspendLoad) {
        ResumeThread(thread.get());
    }
    std::println(std::cout, "Game process started with PID: {}", pi.dwProcessId);
}
} // namespace

int main() try {
    const z::Config config = ReadConfig();
    CheckUpdates(config);
    StartGame(config);
    std::this_thread::sleep_for(std::chrono::seconds { 1 });
    return 0;
} catch (const std::exception& e) {
    try {
        z::ShowMessageBox(
            "Loader Error",
            std::format("An error occurred:\n{}", e.what()),
            z::MessageBoxIcon::Error,
            z::MessageBoxButton::Ok,
            z::MessageBoxDefaultButton::Button1
        );
    } catch (...) {}
    return 1;
}
