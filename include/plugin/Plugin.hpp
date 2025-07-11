#pragma once

#include "plugin/Config.hpp"
#include "plugin/components/CursorState.hpp"
#include "plugin/components/FovUnlocker.hpp"
#include "plugin/components/FpsUnlocker.hpp"
#include "plugin/components/PersistentObject.hpp"
#include "plugin/components/VirtualKeyState.hpp"
#include "plugin/components/WindowState.hpp"
#include "plugin/interfaces/IMediator.hpp"

#include <filesystem>

namespace z3lx::plugin {
namespace detail {
template <typename Derived>
using Mediator = IMediator<
    Derived,
    FpsUnlocker,
    FovUnlocker,
    PersistentObject<Config>,
    WindowState,
    CursorState,
    VirtualKeyState
>;
} // namespace detail

class Plugin final : public detail::Mediator<Plugin> {
public:
    explicit Plugin(std::filesystem::path configFilePath);
    ~Plugin() noexcept;

    void Start();
    void Notify(const OnPersistentObjectChange<Config>& event);
    void Notify(const OnVirtualKeyDown& event);
    void Notify(const OnCursorVisibilityChange& event);
    void Notify(const OnWindowFocusChange& event);

private:
    void ApplyConfig();
    void UpdateHookState();

    std::filesystem::path configFilePath;
};
} // namespace z3lx::plugin
