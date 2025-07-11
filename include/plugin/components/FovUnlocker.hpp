#pragma once

#include "plugin/interfaces/IComponent.hpp"

namespace z3lx::plugin {
class FovUnlocker final : public IComponent<FovUnlocker> {
public:
    FovUnlocker() noexcept;
    ~FovUnlocker() noexcept;

    void Start();

    [[nodiscard]] bool IsHooked() const noexcept;
    void Hook(bool hook);

    [[nodiscard]] bool IsEnabled() const noexcept;
    void Enable(bool enable) noexcept;

    [[nodiscard]] int GetOverrideFov() const noexcept;
    void SetOverrideFov(int overrideFov) noexcept;

    [[nodiscard]] float GetSmoothing() const noexcept;
    void SetSmoothing(float smoothing) noexcept;
};
} // namespace z3lx::plugin
