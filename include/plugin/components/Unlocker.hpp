#pragma once

#include "plugin/interfaces/IComponent.hpp"

namespace z3lx::gfu {
class Unlocker final : public IComponent<Unlocker> {
public:
    Unlocker() noexcept;
    ~Unlocker() noexcept;

    void Start();

    [[nodiscard]] bool Hooked() const noexcept;
    void Hooked(bool value);

    [[nodiscard]] bool Enabled() const noexcept;
    void Enabled(bool value) noexcept;

    [[nodiscard]] int FieldOfView() const noexcept;
    void FieldOfView(int value) noexcept;

    [[nodiscard]] float Smoothing() const noexcept;
    void Smoothing(float value) noexcept;
};
} // namespace z3lx::gfu
