#include "common/Constants.hpp"
#include "loader/Config.hpp"
#include "loader/Version.hpp"
#include "util/Type.hpp"
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
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include <Windows.h>

namespace fs = std::filesystem;
namespace z {
using namespace z3lx::common;
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
        executableName = z::glGameFileName;
    } else if (wil::unique_hkey cnKey {};
        SUCCEEDED(wil::reg::open_unique_key_nothrow(
            HKEY_CURRENT_USER,
            LR"(SOFTWARE\miHoYo\HYP\1_1\hk4e_cn)",
            cnKey
        ))) {
        key = std::move(cnKey);
        executableName = z::cnGameFileName;
    } else {
        THROW_WIN32(ERROR_FILE_NOT_FOUND);
    }

    return std::filesystem::path {
        wil::reg::get_value_string(key.get(), L"GameInstallPath")
    } / executableName;
}

template <typename InvalidConfigCallback, typename InvalidFilePathCallback>
z::Config ReadConfig(
    const std::wstring_view configFilePath,
    InvalidConfigCallback invalidConfigCallback,
    InvalidFilePathCallback invalidFilePathCallback) {
    z::Config config {};
    std::vector<uint8_t> buffer {};
    const wil::unique_hfile configFile = wil::open_or_create_file(
        configFilePath.data()
    );

    bool changed = false;

    try {
        z::ReadFile(configFile.get(), buffer);
        config.Deserialize(buffer);
    } catch (...) {
        changed = true;
        invalidConfigCallback(config);
    }

    const auto isValidFilePath = [](const fs::path& path) {
        return fs::exists(path) && fs::is_regular_file(path);
    };

    if (const fs::path fileName = config.gamePath.filename();
        !isValidFilePath(config.gamePath) || (
        fileName != z::glGameFileName &&
        fileName != z::cnGameFileName)) try {
        changed = true;
        config.gamePath = GetGamePath();
    } catch (...) {
        invalidFilePathCallback(&z::Config::gamePath, config.gamePath);
    }

    for (fs::path& dllPath : config.dllPaths) {
        if (!isValidFilePath(dllPath) ||
            dllPath.extension() != L".dll") {
            changed = true;
            invalidFilePathCallback(&z::Config::dllPaths, dllPath);
        }
    }

    if (changed) {
        config.Serialize(buffer);
        z::WriteFile(configFile.get(), buffer);
    }

    for (fs::path& dllPath : config.dllPaths) {
        dllPath = fs::absolute(dllPath.make_preferred());
    }

    return config;
}

template <typename UpdateCheckCallback>
void CheckUpdates(
    const z::Config& config,
    UpdateCheckCallback updateCheckCallback) {
    if (!config.checkUpdates) {
        return;
    }

    const z::Version currentVersion = z::GetCurrentVersion();
    const z::Version latestVersion = z::GetLatestVersion();
    if (currentVersion < latestVersion) {
        updateCheckCallback(currentVersion, latestVersion);
    }
}

void StartGame(const z::Config& config) {
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
}
} // namespace

int main() try {
    const auto loggingCallback = [](const wil::FailureInfo& info) noexcept {
        std::array<wchar_t, 2048> buffer {};
        const HRESULT result = wil::GetFailureLogString(
            buffer.data(),
            buffer.size(),
            info
        );
        if (SUCCEEDED(result)) {
            std::wcerr << buffer.data();
        }
    };
    wil::SetResultLoggingCallback(loggingCallback);

    std::println(std::cout, "Reading configuration...");
    constexpr auto configFilePath = L"loader_config.json";
    const auto invalidConfigCallback = [](z::Config& config) {
        const z::MessageBoxResult result = z::ShowMessageBox(
            "Loader",
            "Failed to read configuration file.\n"
            "Proceeding with default configuration.",
            z::MessageBoxIcon::Warning,
            z::MessageBoxButton::OkCancel
        );
        if (result == z::MessageBoxResult::Cancel) {
            std::exit(0);
        }
        config = {};
    };
    const auto invalidFilePathCallback = [](auto member, fs::path& path) {
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
        if (z::OffsetOf(member) == z::OffsetOf(&z::Config::gamePath)) {
            constexpr z::Filter filters[] {
                { z::glGameFileName, z::glGameFileName },
                { z::cnGameFileName, z::cnGameFileName }
            };
            path = z::OpenFileDialogue(filters);
        } else if (z::OffsetOf(member) == z::OffsetOf(&z::Config::dllPaths)) {
            constexpr z::Filter filters[] {
                { L"Dynamic Link Library (*.dll)", L"*.dll" }
            };
            path = z::OpenFileDialogue(filters);
        }
    };
    const z::Config config = ReadConfig(
        configFilePath,
        invalidConfigCallback,
        invalidFilePathCallback
    );

    std::println(std::cout, "Checking for updates...");
    const auto updateCheckCallback = [](
        const z::Version& currentVersion,
        const z::Version& latestVersion) {
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
            z::OpenUrl(
                "https://github.com/z3lx/genshin-fov-unlock/releases/latest"
            );
        }
        std::exit(0);
    };
    try {
        CheckUpdates(config, updateCheckCallback);
    } catch (...) {
        LOG_CAUGHT_EXCEPTION();
        std::println(std::cerr, "Update check failed, skipping");
    }

    std::println(std::cout, "Starting game process...");
    StartGame(config);
    std::println(std::cout, "Game process started successfully");
    std::this_thread::sleep_for(std::chrono::seconds { 1 });

    return 0;
} catch (const std::exception& e) {
    LOG_CAUGHT_EXCEPTION();
    try {
        z::ShowMessageBox(
            "Loader",
            std::format("An error occurred:\n{}", e.what()),
            z::MessageBoxIcon::Error,
            z::MessageBoxButton::Ok
        );
    } catch (...) {}
    return 1;
}
