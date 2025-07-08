#include "plugin/Logging.hpp"
#include "plugin/Plugin.hpp"
#include "util/win/Loader.hpp"

#include <wil/result.h>

#include <thread>

#include <Windows.h>

namespace {
void Initialize() try {
    const std::filesystem::path currentPath =
        z3lx::util::GetCurrentModuleFilePath().parent_path();
    wil::SetResultLoggingCallback(z3lx::plugin::GetLoggingCallback(
        currentPath / "plugin_log.txt"
    ));
    static z3lx::plugin::Plugin plugin {
        currentPath / "plugin_config.json"
    };
} CATCH_LOG()
} // namespace

BOOL WINAPI DllMain (
    const HINSTANCE hinstDLL,
    const DWORD fdwReason,
    const LPVOID lpReserved) try {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hinstDLL);
        std::thread { Initialize }.detach();
    }
    return TRUE;
} catch (...) {
    LOG_CAUGHT_EXCEPTION();
    SetLastError(wil::ResultFromCaughtException());
    return FALSE;
}
