#pragma once

#include "plugin/interfaces/ComponentBase.hpp"
#include "util/Type.hpp"

#include <wil/result.h>

#include <chrono>
#include <cstdint>
#include <thread>

#define COMPONENTBASE_TEMPLATE                                                     \
    template <typename Derived, typename... Components>

#define COMPONENTBASE                                                              \
    z3lx::plugin::ComponentBase<Derived, Components...>

namespace z3lx::plugin {
namespace detail {
template <typename Component>
struct ComponentStorage {
    Component value {};
};
} // namespace detail

COMPONENTBASE_TEMPLATE
COMPONENTBASE::ComponentBase() = default;

COMPONENTBASE_TEMPLATE
COMPONENTBASE::~ComponentBase() noexcept = default;

COMPONENTBASE_TEMPLATE
void COMPONENTBASE::Run(const uint16_t frequency) try {
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

COMPONENTBASE_TEMPLATE
constexpr void COMPONENTBASE::Start() {}

COMPONENTBASE_TEMPLATE
constexpr void COMPONENTBASE::Update() {}

COMPONENTBASE_TEMPLATE
template <typename Component>
Component& COMPONENTBASE::GetComponent() noexcept {
    return detail::ComponentStorage<Component>::value;
}

COMPONENTBASE_TEMPLATE
template <typename Component>
const Component& COMPONENTBASE::GetComponent() const noexcept {
    return detail::ComponentStorage<Component>::value;
}

COMPONENTBASE_TEMPLATE
void COMPONENTBASE::StartComponent() try {
    (GetComponent<Components>().StartComponent(), ...);
    static_cast<Derived*>(this)->Start();
} CATCH_THROW_NORMALIZED_MSG(
    "From Start method in component %hs",
    util::GetTypeName<Derived>()
)

COMPONENTBASE_TEMPLATE
void COMPONENTBASE::UpdateComponent() try {
    (GetComponent<Components>().UpdateComponent(), ...);
    static_cast<Derived*>(this)->Update();
} CATCH_THROW_NORMALIZED_MSG(
    "From Update method in component %hs",
    util::GetTypeName<Derived>()
)
} // namespace z3lx::plugin

#undef COMPONENTBASE_TEMPLATE
#undef COMPONENTBASE
