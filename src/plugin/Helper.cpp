module;

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

module plugin;

import :Helper;

import common;
import pwu;
import std;

namespace z3lx::plugin {
GameModuleContext GetGameModuleContext() {
    if (const HMODULE osModule = GetModuleHandleW(common::osGameFileName);
        osModule != nullptr) {
        return GameModuleContext {
            .module = osModule,
            .region = GameRegion::OS
        };
    }
    if (const HMODULE cnModule = GetModuleHandleW(common::cnGameFileName);
        cnModule != nullptr) {
        return GameModuleContext {
            .module = cnModule,
            .region = GameRegion::CN
        };
    }
    pwu::ThrowWin32Error(ERROR_MOD_NOT_FOUND);
}
} // namespace z3lx::plugin
