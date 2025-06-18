#include "plugin/Logging.hpp"
#include "plugin/Plugin.hpp"

#include <wil/result.h>

#include <memory>
#include <thread>

#include <Windows.h>

BOOL WINAPI DllMain (
    const HINSTANCE hinstDLL,
    const DWORD fdwReason,
    const LPVOID lpReserved) try {
    static std::unique_ptr<z3lx::gfu::Plugin> plugin {};

    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hinstDLL);
        std::thread { []() {
            try {
                const auto callback = z3lx::gfu::GetLoggingCallback();
                wil::SetResultLoggingCallback(callback);
                plugin = std::make_unique<z3lx::gfu::Plugin>();
            } CATCH_LOG()
        } }.detach();
        break;

    case DLL_PROCESS_DETACH:
        plugin = nullptr;
        break;

    default: break;
    }

    return TRUE;
} catch (...) {
    LOG_CAUGHT_EXCEPTION();
    SetLastError(wil::ResultFromCaughtException());
    return FALSE;
}
