#pragma once

#include "plugin/interfaces/Common.hpp"

#include <atomic>
#include <thread>

namespace z3lx::gfu {
namespace detail {
template <typename Component>
struct ComponentStorage;
} // namespace detail

IMEDIATOR_TEMPLATE
class IMediator : detail::ComponentStorage<Components>... {
    ICOMPONENT_TEMPLATE
    friend class IComponent;

public:
    IMediator();
    ~IMediator() noexcept;

protected:
    template <typename Component>
    [[nodiscard]] Component& GetComponent() noexcept;

    template <typename Component>
    [[nodiscard]] const Component& GetComponent() const noexcept;

private:
    void UpdateLoop() noexcept;

    void BindComponents() noexcept;
    void StartComponents();
    void UpdateComponents();

    void StartMediator();
    void UpdateMediator();
    template <typename Event>
    static void NotifyMediator(void* instance, const Event& event);

    std::atomic<bool> stopFlag;
    std::thread thread;
};
} // namespace z3lx::gfu

#include "plugin/interfaces/IMediatorInl.hpp"
