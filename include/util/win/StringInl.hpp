#pragma once

#include "util/Math.hpp"
#include "util/win/String.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>

#include <wil/result.h>

#include <Windows.h>

namespace z3lx::util {
namespace detail {
template <
    typename InputContiguousContainer,
    typename OutputContiguousContainer,
    typename InputChunkAddressCallable,
    typename InputChunkCharCountCallable
>
void U16ToCodePage(
    const UINT codePage,
    const InputContiguousContainer& inputBuffer,
    OutputContiguousContainer& outputBuffer,
    InputChunkAddressCallable inputChunkAddressCallable,
    InputChunkCharCountCallable inputChunkCharCountCallable) {
    using InputElementT = typename InputContiguousContainer::value_type;
    const size_t inputCharCount =
        inputBuffer.size() * sizeof(InputElementT) / sizeof(wchar_t);

    // Calculate output buffer size
    size_t outputSize = 0;
    for (size_t inputCharOffset = 0; inputCharOffset < inputCharCount;) {
        const LPCWCH inputChunkAddress = inputChunkAddressCallable(
            inputBuffer,
            inputCharOffset
        );
        const int inputChunkCharCount = inputChunkCharCountCallable(
            inputBuffer,
            inputCharOffset
        );
        const int outputChunkSize = WideCharToMultiByte(
            codePage,
            0,
            inputChunkAddress,
            inputChunkCharCount,
            nullptr,
            0,
            nullptr,
            nullptr
        );
        THROW_LAST_ERROR_IF(outputChunkSize == 0);
        inputCharOffset += inputChunkCharCount;
        outputSize += outputChunkSize;
    }

    using OutputElementT = typename OutputContiguousContainer::value_type;
    const size_t outputElementCount = CeilDiv(
        outputSize,
        sizeof(OutputElementT)
    );
    outputBuffer.resize(outputElementCount);

    // Convert to code page
    size_t outputSizeOffset = 0;
    for (size_t inputCharOffset = 0; inputCharOffset < inputCharCount;) {
        const LPCWCH inputChunkAddress = inputChunkAddressCallable(
            inputBuffer,
            inputCharOffset
        );
        const int inputChunkCharCount = inputChunkCharCountCallable(
            inputBuffer,
            inputCharOffset
        );
        const auto outputChunkAddress =
            reinterpret_cast<char*>(outputBuffer.data()) +
            outputSizeOffset;
        const auto outputChunkSize = static_cast<int>((std::min)(
            static_cast<size_t>((std::numeric_limits<int>::max)()),
            outputSize - outputSizeOffset
        ));
        const int bytesWritten = WideCharToMultiByte(
            codePage,
            0,
            inputChunkAddress,
            inputChunkCharCount,
            outputChunkAddress,
            outputChunkSize,
            nullptr,
            nullptr
        );
        THROW_LAST_ERROR_IF(bytesWritten == 0);
        inputCharOffset += inputChunkCharCount;
        outputSizeOffset += bytesWritten;
    }
}

template <
    typename InputContiguousContainer,
    typename OutputContiguousContainer,
    typename InputChunkAddressCallable,
    typename InputChunkSizeCallable
>
void CodePageToU16(
    const UINT codePage,
    const InputContiguousContainer& inputBuffer,
    OutputContiguousContainer& outputBuffer,
    InputChunkAddressCallable inputChunkAddressCallable,
    InputChunkSizeCallable inputChunkSizeCallable) {
    using InputElementT = typename InputContiguousContainer::value_type;
    const size_t inputSize = inputBuffer.size() * sizeof(InputElementT);

    // Calculate output buffer char count
    size_t outputCharCount = 0;
    for (size_t inputSizeOffset = 0; inputSizeOffset < inputSize;) {
        const LPCCH inputChunkAddress = inputChunkAddressCallable(
            inputBuffer,
            inputSizeOffset
        );
        const int inputChunkSize = inputChunkSizeCallable(
            inputBuffer,
            inputSizeOffset
        );
        const int outputChunkCharCount = MultiByteToWideChar(
            codePage,
            0,
            inputChunkAddress,
            inputChunkSize,
            nullptr,
            0
        );
        THROW_LAST_ERROR_IF(outputChunkCharCount == 0);
        inputSizeOffset += inputChunkSize;
        outputCharCount += outputChunkCharCount;
    }

    using OutputElementT = typename OutputContiguousContainer::value_type;
    const size_t outputElementCount = CeilDiv(
        outputCharCount * sizeof(wchar_t),
        sizeof(OutputElementT)
    );
    outputBuffer.resize(outputElementCount);

    // Convert to UTF-16
    size_t outputCharCountOffset = 0;
    for (size_t inputSizeOffset = 0; inputSizeOffset < inputSize;) {
        const LPCCH inputChunkAddress = inputChunkAddressCallable(
            inputBuffer,
            inputSizeOffset
        );
        const int inputChunkSize = inputChunkSizeCallable(
            inputBuffer,
            inputSizeOffset
        );
        const auto outputChunkAddress =
            reinterpret_cast<wchar_t*>(outputBuffer.data()) +
            outputCharCountOffset;
        const auto outputChunkCharCount = static_cast<int>((std::min)(
            static_cast<size_t>((std::numeric_limits<int>::max)()),
            outputCharCount - outputCharCountOffset
        ));
        const int charsWritten = MultiByteToWideChar(
            codePage,
            0,
            inputChunkAddress,
            inputChunkSize,
            outputChunkAddress,
            outputChunkCharCount
        );
        THROW_LAST_ERROR_IF(charsWritten == 0);
        inputSizeOffset += inputChunkSize;
        outputCharCountOffset += charsWritten;
    }
}

template <typename InputContiguousContainer>
LPCCH U8InputChunkAddress(
    const InputContiguousContainer& inputBuffer,
    const size_t viewSizeOffset) {
    return reinterpret_cast<const char*>(inputBuffer.data()) +
        viewSizeOffset;
};

// Continuation bytes handling to avoid splitting characters
template <typename InputContiguousContainer>
int U8InputChunkSize(
    const InputContiguousContainer& inputBuffer,
    const size_t inputSizeOffset) {
    // 1 to 1 byte to character conversion ratio worst case scenario
    constexpr int maxInputChunkSize = (std::numeric_limits<int>::max)();

    using InputElementT = typename InputContiguousContainer::value_type;
    const size_t inputSize = inputBuffer.size() * sizeof(InputElementT);
    const size_t remainingInputSize = inputSize - inputSizeOffset;

    if (remainingInputSize <= maxInputChunkSize) {
        return static_cast<int>(remainingInputSize);
    }

    const auto isContinuationByte = [](const char byte) -> bool {
        return (byte & 0xC0) == 0x80;
    };
    const char* lastChar =
        reinterpret_cast<const char*>(inputBuffer.data()) +
        inputSizeOffset + maxInputChunkSize - 1;
    uint8_t dropped = 0;
    if (isContinuationByte(lastChar[1])) {
        for (uint8_t i = 0; i < 3; ++i) {
            ++dropped;
            if (!isContinuationByte(lastChar[-i])) {
                break;
            }
        }
    }
    return maxInputChunkSize - dropped;
};

template <typename InputContiguousContainer>
LPCWCH U16InputChunkAddress(
    const InputContiguousContainer& inputBuffer,
    const size_t inputCharOffset) {
    return reinterpret_cast<const wchar_t*>(inputBuffer.data()) +
        inputCharOffset;
};

// Surrogate pair handling to avoid splitting characters
template <typename InputContiguousContainer>
int U16InputChunkCharCount(
    const InputContiguousContainer& inputBuffer,
    const size_t inputCharOffset) {
    // 1 to 3 character to byte conversion ratio worst case scenario
    constexpr int maxInputChunkCharCount =
        (std::numeric_limits<int>::max)() / 3;

    using InputElementT = typename InputContiguousContainer::value_type;
    const size_t inputCharCount =
        inputBuffer.size() * sizeof(InputElementT) / sizeof(wchar_t);
    const size_t remainingInputCharCount = inputCharCount - inputCharOffset;

    if (remainingInputCharCount <= maxInputChunkCharCount) {
        return static_cast<int>(remainingInputCharCount);
    }

    const wchar_t* lastChar =
        reinterpret_cast<const wchar_t*>(inputBuffer.data()) +
        inputCharOffset + maxInputChunkCharCount - 1;
    return IS_HIGH_SURROGATE(*lastChar) ?
        maxInputChunkCharCount - 1 : maxInputChunkCharCount;
};
} // namespace detail

template <typename InputContiguousContainer, typename OutputContiguousContainer>
void U16ToU8(
    const InputContiguousContainer& inputBuffer,
    OutputContiguousContainer& outputBuffer) {
    detail::U16ToCodePage(
        CP_UTF8,
        inputBuffer,
        outputBuffer,
        detail::U16InputChunkAddress<InputContiguousContainer>,
        detail::U16InputChunkCharCount<InputContiguousContainer>
    );
}

template <typename InputContiguousContainer, typename OutputContiguousContainer>
OutputContiguousContainer U16ToU8(
    const InputContiguousContainer& inputBuffer) {
    OutputContiguousContainer outputBuffer;
    U16ToU8(inputBuffer, outputBuffer);
    return outputBuffer;
}

template <typename InputContiguousContainer, typename OutputContiguousContainer>
void U8ToU16(
    const InputContiguousContainer& inputBuffer,
    OutputContiguousContainer& outputBuffer) {
    detail::CodePageToU16(
        CP_UTF8,
        inputBuffer,
        outputBuffer,
        detail::U8InputChunkAddress<InputContiguousContainer>,
        detail::U8InputChunkSize<InputContiguousContainer>
    );
}

template <typename InputContiguousContainer, typename OutputContiguousContainer>
OutputContiguousContainer U8ToU16(
    const InputContiguousContainer& inputBuffer) {
    OutputContiguousContainer outputBuffer;
    U8ToU16(inputBuffer, outputBuffer);
    return outputBuffer;
}
} // namespace z3lx::util
