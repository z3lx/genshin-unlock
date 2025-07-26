#include "loader/Config.hpp"
#include "loader/Version.hpp"
#include "util/win/Dialogue.hpp"
#include "util/win/File.hpp"
#include "util/win/Loader.hpp"
#include "util/win/Shell.hpp"
#include "util/win/String.hpp"
#include "util/win/Version.hpp"

#include <wil/filesystem.h>
#include <wil/registry.h>
#include <wil/resource.h>
#include <wil/result.h>

#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <format>
#include <iostream>
#include <print>
#include <span>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <Windows.h>

namespace z {
using namespace z3lx::loader;
using namespace z3lx::util;
} // namespace z

namespace {
std::filesystem::path GetGamePath() {
    wil::unique_hkey key {};
    const wchar_t* executableName = nullptr;

    if (wil::unique_hkey glKey {};
        SUCCEEDED(wil::reg::open_unique_key_nothrow(
            HKEY_CURRENT_USER,
            LR"(SOFTWARE\Cognosphere\HYP\1_0\hk4e_global)",
            glKey
        ))) {
        key = std::move(glKey);
        executableName = L"GenshinImpact.exe";
    } else if (wil::unique_hkey cnKey {};
        SUCCEEDED(wil::reg::open_unique_key_nothrow(
            HKEY_CURRENT_USER,
            LR"(SOFTWARE\miHoYo\HYP\1_1\hk4e_cn)",
            cnKey
        ))) {
        key = std::move(cnKey);
        executableName = L"YuanShen.exe";
    } else {
        THROW_WIN32(ERROR_FILE_NOT_FOUND);
    }

    return std::filesystem::path {
        wil::reg::get_value_string(key.get(), L"GameInstallPath")
    } / executableName;
}

z::Config ReadConfig(z::Config& config) {
    namespace fs = std::filesystem;
    std::println(std::cout, "Reading configuration...");

    std::vector<uint8_t> buffer {};
    const fs::path configFilePath { L"loader_config.json" };
    const wil::unique_hfile configFile =
        wil::open_or_create_file(configFilePath.c_str());

    const auto serializeConfig = [&] {
        config.Serialize(buffer);
        z::WriteFile(configFile.get(), buffer);
    };

    try {
        z::ReadFile(configFile.get(), buffer);
        config.Deserialize(buffer);
    } catch (const std::exception& e) {
        const z::MessageBoxResult result = z::ShowMessageBox(
            "Loader",
            std::format(
                "Failed to read configuration file '{}'.\n"
                "{}\n"
                "Proceeding with default configuration.",
                configFilePath.string(),
                e.what()
            ),
            z::MessageBoxIcon::Warning,
            z::MessageBoxButton::OkCancel
        );
        if (result == z::MessageBoxResult::Cancel) {
            std::exit(0);
        }
        serializeConfig();
    }

    const auto isValidFilePath = [](const fs::path& path) {
        return fs::exists(path) && fs::is_regular_file(path);
    };

    constexpr auto glGameFileName = L"GenshinImpact.exe";
    constexpr auto cnGameFileName = L"YuanShen.exe";
    const auto isValidGamePath = [isValidFilePath](const fs::path& path) {
        const fs::path fileName = path.filename();
        return isValidFilePath(path) && (
            fileName == glGameFileName ||
            fileName == cnGameFileName
        );
    };

    const auto isValidDllPath = [isValidFilePath](const fs::path& path) {
        return isValidFilePath(path) && path.extension() == L".dll";
    };

    const auto locatePath = [](
        fs::path& path,
        std::span<const z::Filter> filters) {
        const z::MessageBoxResult result = z::ShowMessageBox(
            "Loader",
            std::format(
                "The path '{}' is invalid or does not exist.\n"
                "Locate it using File Explorer?",
                path.string()
            ),
            z::MessageBoxIcon::Warning,
            z::MessageBoxButton::YesNo
        );
        if (result == z::MessageBoxResult::No) {
            std::exit(0);
        }
        path = z::OpenFileDialogue(filters);
    };

    const auto normalizePath = [](fs::path& path) {
        path = fs::absolute(path.make_preferred());
    };

    if (!isValidGamePath(config.gamePath)) {
        try {
            config.gamePath = GetGamePath();
        } catch (...) {
            constexpr z::Filter filters[] {
                { glGameFileName, glGameFileName },
                { cnGameFileName, cnGameFileName }
            };
            locatePath(config.gamePath, filters);
        }
        serializeConfig();
    } else {
        normalizePath(config.gamePath);
    }

    for (auto& dllPath : config.dllPaths) {
        if (!isValidDllPath(dllPath)) {
            constexpr z::Filter filters[] {
                { L"Dynamic Link Library (*.dll)", L"*.dll" }
            };
            locatePath(dllPath, filters);
            serializeConfig();
        } else {
            normalizePath(dllPath);
        }
    }

    return config;
}

void CheckUpdates(const z::Config& config) try {
    if (!config.checkUpdates) {
        return;
    }

    std::println(std::cout, "Checking for updates...");
    const z::Version currentVersion = z::GetCurrentVersion();
    const z::Version latestVersion = z::GetLatestVersion();
    if (currentVersion >= latestVersion) {
        return;
    }

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
        constexpr auto url =
            "https://github.com/z3lx/genshin-fov-unlock/releases/latest";
        z::OpenUrl(url);
    }
    std::exit(0);
} catch (...) {
    LOG_CAUGHT_EXCEPTION();
    std::println(std::cerr, "Update check failed, skipping");
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

        std::wstring additionalArgs {};
        z::U8ToU16(config.additionalArgs, additionalArgs);

        return std::format(
            L"-monitor {} {} -screen-width {} -screen-height {} {} {} ",
            config.monitorIndex,
            modeArgs,
            config.screenWidth,
            config.screenHeight,
            mobileArgs,
            additionalArgs
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
    std::println(std::cout, "Game process started with PID {}", pi.dwProcessId);
}
} // namespace

int main() try {
    wil::SetResultLoggingCallback([] (const wil::FailureInfo& info) noexcept {
        std::array<wchar_t, 2048> buffer {};
        const HRESULT result = wil::GetFailureLogString(
            buffer.data(),
            buffer.size(),
            info
        );
        if (SUCCEEDED(result)) {
            std::wcerr << buffer.data() << std::endl;
        }
    });

    z::Config config {};
    ReadConfig(config);
    CheckUpdates(config);
    StartGame(config);
    std::this_thread::sleep_for(std::chrono::seconds { 1 });
    return 0;
} catch (const std::exception& e) {
    LOG_CAUGHT_EXCEPTION();
    try {
        z::ShowMessageBox(
            "Loader Error",
            std::format("An error occurred:\n{}", e.what()),
            z::MessageBoxIcon::Error,
            z::MessageBoxButton::Ok
        );
    } catch (...) {}
    return 1;
}
