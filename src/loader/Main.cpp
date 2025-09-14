#include <wil/filesystem.h>
#include <wil/registry.h>
#include <wil/resource.h>

#include <Windows.h>

import common;
import loader;
import pwu;
import std;
import util;

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
        executableName = z::osGameFileName;
    } else if (wil::unique_hkey cnKey {};
        SUCCEEDED(wil::reg::open_unique_key_nothrow(
            HKEY_CURRENT_USER,
            LR"(SOFTWARE\miHoYo\HYP\1_1\hk4e_cn)",
            cnKey
        ))) {
        key = std::move(cnKey);
        executableName = z::cnGameFileName;
    } else {
        pwu::ThrowWin32Error(ERROR_FILE_NOT_FOUND);
    }

    return std::filesystem::path {
        wil::reg::get_value_string(key.get(), L"GameInstallPath")
    } / executableName;
}

z::Version GetGameVersion() {
    constexpr std::wstring_view regKeyPaths[] = {
        { LR"(SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\hk4e_global_1_0_VYTpXlbWo8_production)" },
        { LR"(SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\hk4e_cn_1_1_jGHBHlcOq1_production)" }
    };
    for (const std::wstring_view regKeyPath : regKeyPaths) {
        wil::unique_hkey uninstallRegKey {};
        const HRESULT hr = wil::reg::open_unique_key_nothrow(
            HKEY_LOCAL_MACHINE,
            regKeyPath.data(),
            uninstallRegKey
        );
        if (FAILED(hr)) {
            continue;
        }
        std::wstring versionString {};
        pwu::CatchThrowTraced([&] {
            versionString = wil::reg::get_value_string(
                uninstallRegKey.get(),
                L"DisplayVersion"
            );
        });
        return z::Version { versionString };
    }
    pwu::ThrowWin32Error(ERROR_FILE_NOT_FOUND);
}

template <typename OnInvalidConfig, typename OnInvalidFilePath>
z::Config ReadConfig(
    const std::wstring_view configFilePath,
    OnInvalidConfig onInvalidConfig,
    OnInvalidFilePath onInvalidFilePath) {
    z::Config config {};
    std::vector<std::uint8_t> buffer {};
    const wil::unique_hfile configFile = wil::open_or_create_file(
        configFilePath.data()
    );

    bool changed = false;

    try {
        pwu::ReadFile(configFile.get(), buffer);
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
        fileName != z::osGameFileName &&
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
        pwu::WriteFile(configFile.get(), buffer);
    }

    for (fs::path& dllPath : config.dllPaths) {
        dllPath = fs::absolute(dllPath.make_preferred());
    }

    return config;
}

template <typename OnIncompatibility>
void CheckCompatibility(OnIncompatibility onIncompatibility) try {
    const pwu::Version rawVersion = pwu::GetFileVersion(
        pwu::GetCurrentModuleFilePath()
    );
    const z::Version modVersion {
        rawVersion.major,
        rawVersion.minor,
        rawVersion.build
    };
    const z::Version gameVersion = GetGameVersion();
    if (modVersion.GetMajor() != gameVersion.GetMajor() ||
        modVersion.GetMinor() != gameVersion.GetMinor()) {
        onIncompatibility(modVersion, gameVersion);
    }
} catch (...) {
    pwu::CatchThrowTraced([] {
        throw std::runtime_error { "Failed to check compatibility" };
    });
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
        pwu::U8ToU16(config.additionalArgs, additionalArgs);

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
    pwu::ThrowIfWin32BoolFalse(CreateProcessW(
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

    pwu::LoadRemoteLibrary(process.get(), config.dllPaths);
    if (config.suspendLoad) {
        ResumeThread(thread.get());
    }
}
} // namespace

int main() try {
    std::println(std::cout, "Reading configuration...");
    constexpr auto configFilePath = L"loader_config.json";
    const auto onInvalidConfig = [](z::Config& config) {
        const pwu::MessageBoxResult result = pwu::ShowMessageBox(
            "Loader",
            "Failed to read configuration file.\n"
            "Proceeding with default configuration.",
            pwu::MessageBoxIcon::Warning,
            pwu::MessageBoxButton::OkCancel
        );
        if (result == pwu::MessageBoxResult::Cancel) {
            std::exit(0);
        }
        config = {};
    };
    const auto onInvalidFilePath = [](auto member, fs::path& path) {
        const pwu::MessageBoxResult result = pwu::ShowMessageBox(
            "Loader",
            std::format(
                "The path '{}' is invalid or does not exist.\n"
                "Locate it using File Explorer?",
                path.string()
            ),
            pwu::MessageBoxIcon::Warning,
            pwu::MessageBoxButton::YesNo
        );
        if (result == pwu::MessageBoxResult::No) {
            std::exit(0);
        }

        const auto offsetOf = []<typename T, typename U>(
            U T::* member) {
            return reinterpret_cast<size_t>(
                &(reinterpret_cast<const volatile T*>(0)->*member)
            );
        };

        if (offsetOf(member) == offsetOf(&z::Config::gamePath)) {
            constexpr pwu::Filter filters[] {
                { z::osGameFileName, z::osGameFileName },
                { z::cnGameFileName, z::cnGameFileName }
            };
            path = pwu::OpenFileDialogue(filters);
        } else if (offsetOf(member) == offsetOf(&z::Config::dllPaths)) {
            constexpr pwu::Filter filters[] {
                { L"Dynamic Link Library (*.dll)", L"*.dll" }
            };
            path = pwu::OpenFileDialogue(filters);
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
        const pwu::MessageBoxResult result = pwu::ShowMessageBox(
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
            pwu::MessageBoxIcon::Information,
            pwu::MessageBoxButton::YesNo
        );
        if (result == pwu::MessageBoxResult::Yes) {
            pwu::OpenUrl(
                "https://github.com/z3lx/genshin-unlock/releases/latest"
            );
        }
        std::exit(0);
    };
    try {
        CheckCompatibility(onIncompatibility);
    } catch (const std::exception& e) {
        std::println(std::cerr, "{}", e.what());
    }

    std::println(std::cout, "Starting game process...");
    StartGame(config);
    std::println(std::cout, "Game process started successfully");
    std::this_thread::sleep_for(std::chrono::seconds { 1 });

    return 0;
} catch (const std::exception& e) {
    std::println(std::cerr, "{}", e.what());
    try {
        pwu::ShowMessageBox(
            "Loader",
            e.what(),
            pwu::MessageBoxIcon::Error,
            pwu::MessageBoxButton::Ok
        );
    } catch (...) {}
    return 1;
}
