#pragma once

#include <atomic>
#include <thread>
#include <type_traits>
#include <vector>

namespace z3lx::gfu {
namespace detail {
template <typename T>
struct Holder;
}

template <typename Event>
class IComponent;

#define IMEDIATOR_TEMPLATE                                                      \
    template <typename Event, typename... Components>                           \
    requires std::is_copy_constructible_v<Event> &&                             \
        (std::derived_from<Components, IComponent<Event>> && ...) &&            \
        (std::is_default_constructible_v<Components> && ...)

#define IMEDIATOR                                                               \
    IMediator<Event, Components...>

IMEDIATOR_TEMPLATE
class IMediator : detail::Holder<Components>... {
public:
    IMediator();
    virtual ~IMediator() noexcept;

protected:
    virtual void Start();
    virtual void Update();
    virtual void Notify(const Event& event);

    template <typename Component>
    [[nodiscard]] Component& GetComponent() noexcept;

    template <typename Component>
    [[nodiscard]] const Component& GetComponent() const noexcept;

private:
    void UpdateLoop() noexcept;

    void InitializeComponents() noexcept;
    void StartComponents();
    void UpdateComponents();

    void StartMediator();
    void UpdateMediator();
    void NotifyMediator();

    std::atomic<bool> stopFlag;
    std::thread thread;
    std::vector<Event> events;
};
} // namespace z3lx::gfu

#include "plugin/interfaces/IMediatorInl.hpp"
