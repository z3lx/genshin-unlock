#pragma once

#include <typeinfo>

#define TRY_CALL_DERIVED(pointer, Class, Method, ...)                           \
    if constexpr (requires {                                                    \
        static_cast<Class*>(pointer)->Method(__VA_ARGS__); }) {                 \
        static_cast<Class*>(pointer)->Method(__VA_ARGS__);                      \
    }

#define IMEDIATOR_TEMPLATE                                                      \
    template <typename Derived, typename... Components>

#define ICOMPONENT_TEMPLATE                                                     \
    template <typename Derived, typename... Events>

namespace z3lx::gfu {
namespace detail {
template <typename T>
const char* GetTypeName() {
    return typeid(T).name();
}
} // namespace detail

IMEDIATOR_TEMPLATE
class IMediator;

ICOMPONENT_TEMPLATE
class IComponent;
} // namespace z3lx::gfu
