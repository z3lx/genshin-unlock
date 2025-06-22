#pragma once

#include "plugin/Events.hpp"
#include "plugin/components/ConfigManager.hpp"
#include "plugin/components/CursorObserver.hpp"
#include "plugin/components/KeyboardObserver.hpp"
#include "plugin/components/Unlocker.hpp"
#include "plugin/components/WindowObserver.hpp"
#include "plugin/interfaces/IMediator.hpp"

#include <vector>

#include <Windows.h>

namespace z3lx::gfu {
namespace detail {
template <typename Derived>
using Mediator = IMediator<
    Derived,
    Unlocker,
    ConfigManager,
    WindowObserver,
    CursorObserver,
    KeyboardObserver
>;
} // namespace detail

class Plugin final : public detail::Mediator<Plugin> {
public:
    Plugin();
    ~Plugin() noexcept;

    void Start();
    void Notify(const OnConfigChange& event);
    void Notify(const OnKeyDown& event);
    void Notify(const OnCursorVisibilityChange& event);
    void Notify(const OnForegroundWindowChange& event);

private:
    void ConsumeState();

    // State
    bool isWindowFocused;
    bool isCursorVisible;
    std::vector<HWND> targetWindows;
    Config config;
};
} // namespace z3lx::gfu
