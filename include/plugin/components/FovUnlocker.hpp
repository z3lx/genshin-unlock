#pragma once

#include "plugin/components/CursorState.hpp"
#include "plugin/components/WindowState.hpp"
#include "plugin/interfaces/IComponent.hpp"

namespace z3lx::plugin {
class FovUnlocker final : public IComponent<
    FovUnlocker,
    CursorState,
    WindowState
> {
public:
    FovUnlocker() noexcept;
    ~FovUnlocker() noexcept;

    void Start();
    void Update();

    [[nodiscard]] bool IsHooked() const noexcept;
    void Hook(bool hook);

    [[nodiscard]] bool IsEnabled() const noexcept;
    void Enable(bool enable) noexcept;

    [[nodiscard]] int GetTargetFov() const noexcept;
    void SetTargetFov(int targetFov) noexcept;

    [[nodiscard]] float GetSmoothing() const noexcept;
    void SetSmoothing(float smoothing) noexcept;
};
} // namespace z3lx::plugin
