#pragma once

#include "plugin/interfaces/IComponent.hpp"

namespace z3lx::plugin {
class FovUnlocker final : public IComponent<FovUnlocker> {
public:
    FovUnlocker() noexcept;
    ~FovUnlocker() noexcept;

    void Start();

    [[nodiscard]] bool IsEnabled() const noexcept;
    void Enable(bool enable);

    [[nodiscard]] int GetTargetFov() const noexcept;
    void SetTargetFov(int targetFov) noexcept;

    [[nodiscard]] float GetSmoothing() const noexcept;
    void SetSmoothing(float smoothing) noexcept;
};
} // namespace z3lx::plugin
