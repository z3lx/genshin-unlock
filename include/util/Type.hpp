#pragma once

#include <cstdint>

namespace z3lx::util {
template <typename T>
const char* GetTypeName() noexcept {
    return typeid(T).name();
}

template <typename T, typename U>
size_t OffsetOf(U T::* member) {
    return reinterpret_cast<size_t>(
        &(reinterpret_cast<const volatile T*>(0)->*member)
    );
}
} // namespace z3lx::util
