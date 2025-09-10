#include "plugin/Helper.hpp"
#include "common/Constants.hpp"

#include <wil/result.h>

#include <Windows.h>

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
    THROW_WIN32(ERROR_MOD_NOT_FOUND);
}
} // namespace z3lx::plugin
