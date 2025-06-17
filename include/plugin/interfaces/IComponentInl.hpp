#pragma once

#include "plugin/interfaces/IComponent.hpp"

#include <wil/result.h>

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

template <typename Event>
void IComponent<Event>::StartComponent() try {
    Start();
} CATCH_THROW_NORMALIZED_MSG("%hs", typeid(*this).name())

template <typename Event>
void IComponent<Event>::UpdateComponent() try {
    Update();
} CATCH_THROW_NORMALIZED_MSG("%hs", typeid(*this).name())
} // namespace z3lx::gfu
