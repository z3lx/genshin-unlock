#include "common/Constants.hpp"
#include "loader/Config.hpp"
#include "util/Type.hpp"
#include "util/Version.hpp"
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

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cwchar>
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

z::Version GetGameVersion() {
    constexpr std::wstring_view targets[] = {
        L"原神",
        L"Genshin Impact"
    };

    const wil::unique_hkey uninstallRegKeys[] {
        wil::reg::open_unique_key(
            HKEY_LOCAL_MACHINE,
            LR"(SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall)"
        ),
        wil::reg::open_unique_key(
            HKEY_LOCAL_MACHINE,
            LR"(SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall)"
        ),
        wil::reg::open_unique_key(
            HKEY_CURRENT_USER,
            LR"(SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall)"
        ),
    };

    for (const auto& uninstallRegKey : uninstallRegKeys) {
        const auto subRegKeyNames = wil::make_range(
            wil::reg::key_iterator { uninstallRegKey.get() },
            wil::reg::key_iterator {}
        );
        for (const auto& subRegKeyName : subRegKeyNames) {
            const wil::unique_hkey subRegKey = wil::reg::open_unique_key(
                uninstallRegKey.get(),
                subRegKeyName.name.c_str()
            );

            wchar_t buffer[1024] {};
            const HRESULT hr = wil::reg::get_value_string_nothrow(
                subRegKey.get(),
                L"DisplayName",
                buffer
            );

            const auto comp = [&buffer](const std::wstring_view t) {
                return std::wcscmp(buffer, t.data()) == 0;
            };
            if (FAILED(hr) || std::ranges::none_of(targets, comp)) {
                continue;
            }

            THROW_IF_FAILED(wil::reg::get_value_string_nothrow(
                subRegKey.get(),
                L"DisplayVersion",
                buffer
            ));
            return z::Version { buffer };
        }
    }
    THROW_WIN32(ERROR_FILE_NOT_FOUND);
}

template <typename OnInvalidConfig, typename OnInvalidFilePath>
z::Config ReadConfig(
    const std::wstring_view configFilePath,
    OnInvalidConfig onInvalidConfig,
    OnInvalidFilePath onInvalidFilePath) {
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
        onInvalidConfig(config);
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
        onInvalidFilePath(&z::Config::gamePath, config.gamePath);
    }

    for (fs::path& dllPath : config.dllPaths) {
        if (!isValidFilePath(dllPath) ||
            dllPath.extension() != L".dll") {
            changed = true;
            onInvalidFilePath(&z::Config::dllPaths, dllPath);
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

template <typename OnIncompatibility>
void CheckCompatibility(OnIncompatibility onIncompatibility) {
    const z::Version modVersion = z::GetFileVersion(
        z::GetCurrentModuleFilePath()
    );
    const z::Version gameVersion = GetGameVersion();
    if (modVersion.GetMajor() != gameVersion.GetMajor() ||
        modVersion.GetMinor() != gameVersion.GetMinor()) {
        onIncompatibility(modVersion, gameVersion);
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
    const auto onInvalidConfig = [](z::Config& config) {
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
    const auto onInvalidFilePath = [](auto member, fs::path& path) {
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
        onInvalidConfig,
        onInvalidFilePath
    );

    std::println(std::cout, "Checking compatibility...");
    const auto onIncompatibility = [](
        const z::Version& modVersion,
        const z::Version& gameVersion) {
        const z::MessageBoxResult result = z::ShowMessageBox(
            "Loader",
            std::format(
                "The installed mod version is not compatible "
                "with the current game version.\n"
                "Mod version: {}\n"
                "Game version: {}\n\n"
                "Open the download page to check for updates?",
                modVersion.ToString(),
                gameVersion.ToString()
            ),
            z::MessageBoxIcon::Information,
            z::MessageBoxButton::YesNo
        );
        if (result == z::MessageBoxResult::Yes) {
            z::OpenUrl(
                "https://github.com/z3lx/genshin-unlock/releases/latest"
            );
        }
        std::exit(0);
    };
    CheckCompatibility(onIncompatibility);

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
