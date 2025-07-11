#pragma once

#include "plugin/interfaces/IComponent.hpp"

namespace z3lx::plugin {
class FpsUnlocker final : public IComponent<FpsUnlocker> {
public:
    FpsUnlocker() noexcept;
    ~FpsUnlocker() noexcept;

    void Start();
    void Update() const;

    [[nodiscard]] bool IsEnabled() const noexcept;
    void Enable(bool enable) noexcept;

    [[nodiscard]] int GetOverrideFps() const noexcept;
    void SetOverrideFps(int overrideFps) noexcept;

private:
    bool isEnabled;
    int overrideFps;
    int* targetFps;
};
} // namespace z3lx::plugin
