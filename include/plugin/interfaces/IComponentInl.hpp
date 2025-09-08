#pragma once

#include "plugin/interfaces/IComponent.hpp"
#include "util/Type.hpp"

#include <wil/result.h>

#include <chrono>
#include <cstdint>
#include <thread>

#define ICOMPONENT_TEMPLATE                                                     \
    template <typename Derived, typename... Components>

#define ICOMPONENT                                                              \
    z3lx::plugin::IComponent<Derived, Components...>

namespace z3lx::plugin {
namespace detail {
template <typename Component>
struct ComponentStorage {
    Component value {};
};
} // namespace detail

ICOMPONENT_TEMPLATE
ICOMPONENT::IComponent() = default;

ICOMPONENT_TEMPLATE
ICOMPONENT::~IComponent() noexcept = default;

ICOMPONENT_TEMPLATE
void ICOMPONENT::Run(const uint16_t frequency) try {
    auto derived = static_cast<Derived*>(this);
    derived->StartComponent();
    while (true) {
        derived->UpdateComponent();
        if (frequency != 0) {
            using Milliseconds = std::chrono::duration<double, std::milli>;
            const Milliseconds duration { 1000.0 / frequency };
            std::this_thread::sleep_for(duration);
        }
    }
} CATCH_THROW_NORMALIZED_MSG(
    "From Run method in component %hs",
    util::GetTypeName<Derived>()
)

ICOMPONENT_TEMPLATE
constexpr void ICOMPONENT::Start() {}

ICOMPONENT_TEMPLATE
constexpr void ICOMPONENT::Update() {}

ICOMPONENT_TEMPLATE
template <typename Component>
Component& ICOMPONENT::GetComponent() noexcept {
    return detail::ComponentStorage<Component>::value;
}

ICOMPONENT_TEMPLATE
template <typename Component>
const Component& ICOMPONENT::GetComponent() const noexcept {
    return detail::ComponentStorage<Component>::value;
}

ICOMPONENT_TEMPLATE
void ICOMPONENT::StartComponent() try {
    (GetComponent<Components>().StartComponent(), ...);
    static_cast<Derived*>(this)->Start();
} CATCH_THROW_NORMALIZED_MSG(
    "From Start method in component %hs",
    util::GetTypeName<Derived>()
)

ICOMPONENT_TEMPLATE
void ICOMPONENT::UpdateComponent() try {
    (GetComponent<Components>().UpdateComponent(), ...);
    static_cast<Derived*>(this)->Update();
} CATCH_THROW_NORMALIZED_MSG(
    "From Update method in component %hs",
    util::GetTypeName<Derived>()
)
} // namespace z3lx::plugin

#undef ICOMPONENT_TEMPLATE
#undef ICOMPONENT
