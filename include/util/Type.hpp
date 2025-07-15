#pragma once

namespace z3lx::util {
template <typename T>
const char* GetTypeName() noexcept {
    return typeid(T).name();
}
} // namespace z3lx::util
