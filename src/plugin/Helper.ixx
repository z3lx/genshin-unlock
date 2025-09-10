module;

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

export module plugin:Helper;

import std;

namespace z3lx::plugin {
enum class GameRegion : std::uint8_t {
    OS,
    CN
};

struct GameModuleContext {
    HMODULE module;
    GameRegion region;
};

GameModuleContext GetGameModuleContext();
} // namespace z3lx::plugin
