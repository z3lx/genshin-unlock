#pragma once

#include "plugin/interfaces/IMediator.hpp"

#include <type_traits>
#include <vector>

namespace z3lx::gfu {
template <typename Event>
class IComponent {
    IMEDIATOR_TEMPLATE
    friend class IMediator;
public:
    IComponent() noexcept;
    virtual ~IComponent() noexcept;

protected:
    virtual void Start();
    virtual void Update();

    void Notify(const Event& event);

private:
    void StartComponent();
    void UpdateComponent();

    std::vector<Event>* events;
};
} // namespace z3lx::gfu

#include "plugin/interfaces/IComponentInl.hpp"
