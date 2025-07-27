#include "plugin/Helper.hpp"
#include "common/Constants.hpp"

#include <wil/result.h>

#include <Windows.h>

namespace z3lx::plugin {
GameModuleContext GetGameModuleContext() {
    if (const HMODULE glModule = GetModuleHandleW(common::glGameFileName);
        glModule != nullptr) {
        return GameModuleContext {
            .module = glModule,
            .region = GameRegion::GL
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
