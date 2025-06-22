#pragma once

#include "plugin/interfaces/Common.hpp"
#include "plugin/interfaces/IMediator.hpp"

#include <wil/result.h>

#include <atomic>
#include <chrono>
#include <thread>

#define IMEDIATOR                                                               \
    IMediator<Derived, Components...>

namespace z3lx::gfu {
namespace detail {
template <typename Component>
struct ComponentStorage {
    Component value {};
};
} // namespace detail

IMEDIATOR_TEMPLATE
IMEDIATOR::IMediator()
    : stopFlag { false }
    , thread { [this]() -> void { UpdateLoop(); } } {}

IMEDIATOR_TEMPLATE
IMEDIATOR::~IMediator() noexcept {
    stopFlag.store(true, std::memory_order_relaxed);
    thread.join();
}

IMEDIATOR_TEMPLATE
template <typename Component>
Component& IMEDIATOR::GetComponent() noexcept {
    return detail::ComponentStorage<Component>::value;
}

IMEDIATOR_TEMPLATE
template <typename Component>
const Component& IMEDIATOR::GetComponent() const noexcept {
    return detail::ComponentStorage<Component>::value;
}

IMEDIATOR_TEMPLATE
void IMEDIATOR::UpdateLoop() noexcept try {
    BindComponents();
    StartComponents();
    StartMediator();

    while (!stopFlag.load(std::memory_order_relaxed)) {
        UpdateComponents();
        UpdateMediator();

        // Wait until the next scheduler tick
        std::this_thread::sleep_for(
            std::chrono::milliseconds { 1 }
        );
    }
} CATCH_LOG_MSG("%hs", detail::GetTypeName<Derived>())

IMEDIATOR_TEMPLATE
void IMEDIATOR::BindComponents() noexcept {
    (GetComponent<Components>().BindComponent(this), ...);
}

IMEDIATOR_TEMPLATE
void IMEDIATOR::StartComponents() {
    (GetComponent<Components>().StartComponent(), ...);
}

IMEDIATOR_TEMPLATE
void IMEDIATOR::UpdateComponents() {
    (GetComponent<Components>().UpdateComponent(), ...);
}

IMEDIATOR_TEMPLATE
void IMEDIATOR::StartMediator() try {
    TRY_CALL_DERIVED(this, Derived, Start);
} CATCH_THROW_NORMALIZED_MSG("%hs", detail::GetTypeName<Derived>())

IMEDIATOR_TEMPLATE
void IMEDIATOR::UpdateMediator() try {
    TRY_CALL_DERIVED(this, Derived, Update);
} CATCH_THROW_NORMALIZED_MSG("%hs", detail::GetTypeName<Derived>())

IMEDIATOR_TEMPLATE
template <typename Event>
void IMEDIATOR::NotifyMediator(void* instance, const Event& event) try {
    TRY_CALL_DERIVED(instance, Derived, Notify, event);
} CATCH_THROW_NORMALIZED_MSG("%hs", detail::GetTypeName<Derived>())
} // namespace z3lx::gfu

#undef IMEDIATOR
