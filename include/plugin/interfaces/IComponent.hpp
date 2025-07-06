#pragma once

#include "plugin/interfaces/Common.hpp"

namespace z3lx::plugin {
namespace detail {
template <typename Event>
struct EventCallbackStorage;
} // namespace detail

ICOMPONENT_TEMPLATE
class IComponent : detail::EventCallbackStorage<Events>... {
    IMEDIATOR_TEMPLATE
    friend class IMediator;

public:
    IComponent() noexcept;
    ~IComponent() noexcept;
    IComponent(const IComponent&) = delete;
    IComponent(IComponent&&) = delete;

protected:
    template <typename Event>
    void Notify(const Event& event);

private:
    template <typename Mediator>
    void BindComponent(Mediator* mediator) noexcept;
    void StartComponent();
    void UpdateComponent();

    void* instance;
};
} // namespace z3lx::gfu

#include "plugin/interfaces/IComponentInl.hpp"
