#pragma once

#include "plugin/interfaces/IComponent.hpp"

namespace z3lx::gfu {
template <typename Event>
IComponent<Event>::IComponent() noexcept
    : events { nullptr } {}

template <typename Event>
IComponent<Event>::~IComponent() noexcept = default;

template <typename Event>
void IComponent<Event>::Start() {}

template <typename Event>
void IComponent<Event>::Update() {}

template <typename Event>
void IComponent<Event>::Notify(const Event& event) {
    events->push_back(event);
}
} // namespace z3lx::gfu
