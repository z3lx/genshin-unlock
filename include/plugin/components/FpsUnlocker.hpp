#pragma once

#include "plugin/interfaces/IComponent.hpp"

namespace z3lx::plugin {
class FpsUnlocker final : public IComponent<FpsUnlocker> {
public:
    FpsUnlocker() noexcept;
    ~FpsUnlocker() noexcept;

    void Start();
    void Update() const noexcept;

    [[nodiscard]] bool IsEnabled() const noexcept;
    void Enable(bool enable) noexcept;

    [[nodiscard]] int GetTargetFps() const noexcept;
    void SetTargetFps(int targetFps) noexcept;

private:
    bool isEnabled;
    int targetFps;
    int* targetFpsPtr;
};
} // namespace z3lx::plugin
