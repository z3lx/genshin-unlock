#pragma once

#include "plugin/Config.hpp"
#include "plugin/components/ConfigManager.hpp"
#include "plugin/components/CursorObserver.hpp"
#include "plugin/components/KeyboardObserver.hpp"
#include "plugin/components/Unlocker.hpp"
#include "plugin/components/WindowObserver.hpp"
#include "plugin/interfaces/IMediator.hpp"

namespace z3lx::gfu {
namespace detail {
template <typename Derived>
using Mediator = IMediator<
    Derived,
    Unlocker,
    ConfigManager<Config>,
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
    void Notify(const OnConfigChange<Config>& event);
    void Notify(const OnKeyDown& event);
    void Notify(const OnCursorVisibilityChange& event);
    void Notify(const OnWindowFocusChange& event);

private:
    void UpdateHookState();
};
} // namespace z3lx::gfu
