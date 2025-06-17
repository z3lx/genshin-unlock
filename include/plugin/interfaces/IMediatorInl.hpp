#pragma once

#include "plugin/interfaces/IMediator.hpp"

#include <atomic>
#include <chrono>
#include <thread>

namespace z3lx::gfu {
namespace detail {
template <typename T>
struct Holder {
    T value;
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
void IMEDIATOR::Start() {}

IMEDIATOR_TEMPLATE
void IMEDIATOR::Update() {}

IMEDIATOR_TEMPLATE
void IMEDIATOR::Notify(const Event& event) {}

IMEDIATOR_TEMPLATE
template <typename Component>
Component& IMEDIATOR::GetComponent() noexcept {
    return detail::Holder<Component>::value;
}

IMEDIATOR_TEMPLATE
template <typename Component>
const Component& IMEDIATOR::GetComponent() const noexcept {
    return detail::Holder<Component>::value;
}

IMEDIATOR_TEMPLATE
void IMEDIATOR::UpdateLoop() noexcept try {
    InitializeComponents();
    StartComponents();
    StartMediator();

    while (!stopFlag.load(std::memory_order_relaxed)) {
        UpdateComponents();
        UpdateMediator();
        NotifyMediator();

        // Wait until the next scheduler tick
        std::this_thread::sleep_for(
            std::chrono::milliseconds { 1 }
        );
    }
} catch (...) {}

IMEDIATOR_TEMPLATE
void IMEDIATOR::InitializeComponents() noexcept {
    ((GetComponent<Components>().events = &events), ...);
}

IMEDIATOR_TEMPLATE
void IMEDIATOR::StartComponents() {
    (static_cast<IComponent<Event>&>(GetComponent<Components>()).Start(), ...);
}

IMEDIATOR_TEMPLATE
void IMEDIATOR::UpdateComponents() {
    (static_cast<IComponent<Event>&>(GetComponent<Components>()).Update(), ...);
}

IMEDIATOR_TEMPLATE
void IMEDIATOR::StartMediator() {
    Start();
}

IMEDIATOR_TEMPLATE
void IMEDIATOR::UpdateMediator() {
    Update();
}

IMEDIATOR_TEMPLATE
void IMEDIATOR::NotifyMediator() {
    for (const Event& event : events) {
        Notify(event);
    }
    events.clear();
}
} // namespace z3lx::gfu
