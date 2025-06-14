#pragma once

#include "components/ConfigManager.hpp"
#include "plugin/Events.hpp"
#include "plugin/interfaces/IMediator.hpp"

#include <vector>

#include <Windows.h>

namespace z3lx::gfu {
class Plugin final : public IMediator<Event> {
public:
    Plugin() noexcept;
    ~Plugin() noexcept override;

private:
    struct Visitor;

    void Start() noexcept override;
    void Notify(const Event& event) noexcept override;

    template <typename Event>
    void Handle(const Event& event) noexcept;
    void ConsumeState() noexcept;

    // State
    bool isUnlockerHooked;
    bool isWindowFocused;
    bool isCursorVisible;
    std::vector<HWND> targetWindows;
    Config config;
};
} // namespace z3lx::gfu
