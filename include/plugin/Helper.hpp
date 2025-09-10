#pragma once

#include <cstdint>

#include <Windows.h>

namespace z3lx::plugin {
enum class GameRegion : uint8_t {
    OS,
    CN
};

struct GameModuleContext {
    HMODULE module;
    GameRegion region;
};

GameModuleContext GetGameModuleContext();
} // namespace z3lx::plugin
