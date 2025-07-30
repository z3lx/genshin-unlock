#pragma once

#include <string>

namespace z3lx::util {
template <typename InputContiguousContainer, typename OutputContiguousContainer>
void U16ToU8(
    const InputContiguousContainer& inputBuffer,
    OutputContiguousContainer& outputBuffer
);

template <
    typename InputContiguousContainer,
    typename OutputContiguousContainer = std::u8string
>
[[nodiscard]] OutputContiguousContainer U16ToU8(
    const InputContiguousContainer& inputBuffer
);

template <typename InputContiguousContainer, typename OutputContiguousContainer>
void U8ToU16(
    const InputContiguousContainer& inputBuffer,
    OutputContiguousContainer& outputBuffer
);

template <
    typename InputContiguousContainer,
    typename OutputContiguousContainer = std::u16string
>
[[nodiscard]] OutputContiguousContainer U8ToU16(
    const InputContiguousContainer& inputBuffer
);
} // namespace z3lx::util

#include "util/win/StringInl.hpp"
