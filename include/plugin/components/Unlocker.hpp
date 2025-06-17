#pragma once

#include "plugin/Events.hpp"
#include "plugin/interfaces/IComponent.hpp"

namespace z3lx::gfu {
class Unlocker final : public IComponent<Event> {
public:
    Unlocker() noexcept;
    ~Unlocker() noexcept override;

    [[nodiscard]] bool Hooked() const noexcept;
    void Hooked(bool value);

    [[nodiscard]] bool Enabled() const noexcept;
    void Enabled(bool value) noexcept;

    [[nodiscard]] int FieldOfView() const noexcept;
    void FieldOfView(int value) noexcept;

    [[nodiscard]] float Smoothing() const noexcept;
    void Smoothing(float value) noexcept;

private:
    void Start() override;
};
} // namespace z3lx::gfu
