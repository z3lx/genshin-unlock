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
using Mediator = IMediator<
    Event,
    Unlocker,
    ConfigManager,
    WindowObserver,
    CursorObserver,
    KeyboardObserver
>;
} // namespace detail

class Plugin final : public detail::Mediator {
public:
    Plugin();
    ~Plugin() noexcept override;

private:
    void Start() override;
    void Notify(const Event& event) override;

    template <typename Event>
    void Handle(const Event& event);
    void ConsumeState();

    // State
    bool isWindowFocused;
    bool isCursorVisible;
    std::vector<HWND> targetWindows;
    Config config;
};
} // namespace z3lx::gfu
