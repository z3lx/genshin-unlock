#pragma once

#include "plugin/Events.hpp"
#include "plugin/interfaces/IComponent.hpp"

namespace z3lx::gfu {
class Unlocker final : public IComponent<Event> {
public:
    Unlocker() noexcept;
    ~Unlocker() noexcept override;

    void SetHook(bool value) const;
    void SetEnable(bool value) const noexcept;
    void SetFieldOfView(int value) noexcept;
    void SetSmoothing(float value) noexcept;

private:
    void Start() override;
};
} // namespace z3lx::gfu
