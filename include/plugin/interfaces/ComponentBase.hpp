#pragma once

#include <cstdint>

namespace z3lx::plugin {
namespace detail {
template <typename Component>
struct ComponentStorage;
} // namespace detail

template <typename Derived, typename... Components>
class ComponentBase : detail::ComponentStorage<Components>... {
    template <typename D, typename... Cs>
    friend class ComponentBase;

public:
    ComponentBase();
    ~ComponentBase() noexcept;
    ComponentBase(const ComponentBase&) = delete;
    ComponentBase(ComponentBase&&) = delete;

    [[noreturn]] void Run(uint16_t frequency = 60);

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

#include "plugin/interfaces/ComponentBaseInl.hpp"
