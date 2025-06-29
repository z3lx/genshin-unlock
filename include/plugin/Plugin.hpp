#pragma once

#include "plugin/Config.hpp"
#include "plugin/components/CursorState.hpp"
#include "plugin/components/PersistentObject.hpp"
#include "plugin/components/Unlocker.hpp"
#include "plugin/components/VirtualKeyState.hpp"
#include "plugin/components/WindowState.hpp"
#include "plugin/interfaces/IMediator.hpp"

namespace z3lx::gfu {
namespace detail {
template <typename Derived>
using Mediator = IMediator<
    Derived,
    Unlocker,
    PersistentObject<Config>,
    WindowState,
    CursorState,
    VirtualKeyState
>;
} // namespace detail

class Plugin final : public detail::Mediator<Plugin> {
public:
    Plugin();
    ~Plugin() noexcept;

    void Start();
    void Notify(const OnPersistentObjectChange<Config>& event);
    void Notify(const OnVirtualKeyDown& event);
    void Notify(const OnCursorVisibilityChange& event);
    void Notify(const OnWindowFocusChange& event);

private:
    void UpdateHookState();
};
} // namespace z3lx::gfu
