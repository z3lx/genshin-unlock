#pragma once

#include "plugin/interfaces/IRunnable.hpp"

#include <cstdint>

namespace z3lx::plugin {
namespace detail {
template <typename Component>
struct ComponentStorage;
} // namespace detail

template <typename Derived, typename... Components>
class IComponent : detail::ComponentStorage<Components>... {
    template <typename D, uint16_t F>
    friend class IRunnable;

    template <typename D, typename... Cs>
    friend class IComponent;

public:
    IComponent();
    ~IComponent() noexcept;
    IComponent(const IComponent&) = delete;
    IComponent(IComponent&&) = delete;

protected:
    constexpr static void Start();
    constexpr static void Update();

    template <typename Component>
    [[nodiscard]] Component& GetComponent() noexcept;

    template <typename Component>
    [[nodiscard]] const Component& GetComponent() const noexcept;

private:
    void StartComponent();
    void UpdateComponent();
};
} // namespace z3lx::plugin

#include "plugin/interfaces/IComponentInl.hpp"
