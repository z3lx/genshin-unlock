#pragma once

#include "plugin/interfaces/Common.hpp"
#include "plugin/interfaces/IComponent.hpp"

#include <wil/result.h>

#define ICOMPONENT                                                              \
    IComponent<Derived, Events...>

namespace z3lx::gfu {
namespace detail {
template <typename Event>
struct EventCallbackStorage {
    void (*value)(void*, const Event& event) = nullptr;
};
} // namespace detail

ICOMPONENT_TEMPLATE
ICOMPONENT::IComponent() noexcept
    : instance { nullptr } {}

ICOMPONENT_TEMPLATE
ICOMPONENT::~IComponent() noexcept = default;

ICOMPONENT_TEMPLATE
template <typename Event>
void ICOMPONENT::Notify(const Event& event) {
    detail::EventCallbackStorage<Event>::value(instance, event);
}

ICOMPONENT_TEMPLATE
template <typename Mediator>
void ICOMPONENT::BindComponent(Mediator* mediator) noexcept {
    instance = mediator;
    ((detail::EventCallbackStorage<Events>::value =
        &Mediator::template NotifyMediator<Events>), ...);
}

ICOMPONENT_TEMPLATE
void ICOMPONENT::StartComponent() try {
    TRY_CALL_DERIVED(this, Derived, Start);
} CATCH_THROW_NORMALIZED_MSG("%hs", detail::GetTypeName<Derived>())

ICOMPONENT_TEMPLATE
void ICOMPONENT::UpdateComponent() try {
    TRY_CALL_DERIVED(this, Derived, Update);
} CATCH_THROW_NORMALIZED_MSG("%hs", detail::GetTypeName<Derived>())
} // namespace z3lx::gfu

#undef ICOMPONENT
