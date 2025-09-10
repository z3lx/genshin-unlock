#define WIN32_LEAN_AND_MEAN
#include <wil/filesystem.h>
#include <wil/resource.h>

#include <Windows.h>

import plugin;
import pwu;
import std;

namespace {
void Initialize() try {
    namespace fs = std::filesystem;
    const fs::path currentPath = pwu::GetCurrentModuleFilePath().parent_path();
    try {
        const fs::path filePath = currentPath / "plugin";
        z3lx::plugin::Plugin { filePath }.Run();
    } catch (const pwu::TracedException& e) {
        const fs::path filePath = currentPath / "plugin_log.txt";
        const wil::unique_hfile fileHandle =
            wil::open_or_truncate_existing_file(filePath.native().c_str());
        const std::string_view trace { e.what() };
        pwu::AppendFile(fileHandle.get(), trace);
    }
} catch (...) {}
} // namespace

BOOL WINAPI DllMain(
    const HINSTANCE hinstDLL,
    const DWORD fdwReason,
    const LPVOID lpReserved) try {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hinstDLL);
        std::thread { Initialize }.detach();
    }
    return TRUE;
} catch (...) {
    return FALSE;
}
