#pragma once

#include "plugin/interfaces/IComponent.hpp"

namespace z3lx::plugin {
class Unlocker final : public IComponent<Unlocker> {
public:
    Unlocker() noexcept;
    ~Unlocker() noexcept;

    void Start();

    [[nodiscard]] bool IsHooked() const noexcept;
    void Hook(bool hook);

    [[nodiscard]] bool IsEnabled() const noexcept;
    void Enable(bool enable) noexcept;

    [[nodiscard]] int GetFieldOfView() const noexcept;
    void SetFieldOfView(int fieldOfView) noexcept;

    [[nodiscard]] float GetSmoothing() const noexcept;
    void SetSmoothing(float smoothing) noexcept;
};
} // namespace z3lx::gfu
