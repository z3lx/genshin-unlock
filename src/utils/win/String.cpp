#include "utils/win/String.hpp"

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <type_traits>

#include <wil/result.h>

#include <Windows.h>

namespace {
template <typename T>
concept AChar = std::same_as<T, char>;

template <typename T>
concept U8Char = std::same_as<T, char8_t> || std::same_as<T, char>;

template <typename T>
concept U16Char = std::same_as<T, char16_t> || std::same_as<T, wchar_t>;

template <typename Ret, typename Callable, typename CharT>
concept ViewChunkGetter = std::is_invocable_r_v<
    Ret, Callable, std::basic_string_view<CharT>, size_t
>;

template <
    typename ViewCharT,
    typename BufferCharT,
    typename GetViewChunkAddressFunc,
    typename GetViewChunkCharCountFunc
>
requires ViewChunkGetter<LPCWCH, GetViewChunkAddressFunc, ViewCharT> &&
    ViewChunkGetter<int, GetViewChunkCharCountFunc, ViewCharT>
std::basic_string<BufferCharT> U16ToCodePage(
    const UINT codePage,
    const std::basic_string_view<ViewCharT> view,
    GetViewChunkAddressFunc getViewChunkAddress,
    GetViewChunkCharCountFunc getViewChunkCharCount) {
    // Calculate buffer size
    size_t bufferSize = 0;
    for (size_t viewCharOffset = 0; viewCharOffset < view.size();) {
        const LPCWCH viewChunkAddress = getViewChunkAddress(
            view,
            viewCharOffset
        );
        const int viewChunkCharCount = getViewChunkCharCount(
            view,
            viewCharOffset
        );
        const int bufferChunkSize = ::WideCharToMultiByte(
            codePage,
            0,
            viewChunkAddress,
            viewChunkCharCount,
            nullptr,
            0,
            nullptr,
            nullptr
        );
        THROW_LAST_ERROR_IF(bufferChunkSize == 0);
        viewCharOffset += viewChunkCharCount;
        bufferSize += bufferChunkSize;
    }

    const size_t bufferCharCount =
        (bufferSize + sizeof(BufferCharT) - 1) / sizeof(BufferCharT);
    std::basic_string<BufferCharT> buffer(bufferCharCount, 0);

    // Convert to code page
    size_t bufferSizeOffset = 0;
    for (size_t viewCharOffset = 0; viewCharOffset < view.size();) {
        const LPCWCH viewChunkAddress = getViewChunkAddress(
            view,
            viewCharOffset
        );
        const int viewChunkCharCount = getViewChunkCharCount(
            view,
            viewCharOffset
        );
        const auto bufferChunkAddress = reinterpret_cast<LPSTR>(
            reinterpret_cast<uint8_t*>(buffer.data()) + bufferSizeOffset
        );
        const auto bufferChunkSize = static_cast<int>((std::min)(
            static_cast<size_t>((std::numeric_limits<int>::max)()),
            buffer.size() * sizeof(BufferCharT) - bufferSizeOffset
        ));
        const int bytesWritten = ::WideCharToMultiByte(
            codePage,
            0,
            viewChunkAddress,
            viewChunkCharCount,
            bufferChunkAddress,
            bufferChunkSize,
            nullptr,
            nullptr
        );
        THROW_LAST_ERROR_IF(bytesWritten == 0);
        viewCharOffset += viewChunkCharCount;
        bufferSizeOffset += bytesWritten;
    }

    return buffer;
}

template <
    typename ViewCharT,
    typename BufferCharT,
    typename GetViewChunkAddressFunc,
    typename GetViewChunkSizeFunc
>
requires ViewChunkGetter<LPCCH, GetViewChunkAddressFunc, ViewCharT> &&
    ViewChunkGetter<int, GetViewChunkSizeFunc, ViewCharT>
std::basic_string<BufferCharT> CodePageToU16(
    const UINT codePage,
    const std::basic_string_view<ViewCharT> view,
    GetViewChunkAddressFunc getViewChunkAddress,
    GetViewChunkSizeFunc getViewChunkSize) {
    // Calculate buffer char count
    size_t bufferCharCount = 0;
    const size_t viewSize = view.size() * sizeof(ViewCharT);
    for (size_t viewSizeOffset = 0; viewSizeOffset < viewSize;) {
        const LPCCH viewChunkAddress = getViewChunkAddress(
            view,
            viewSizeOffset
        );
        const int viewChunkSize = getViewChunkSize(
            view,
            viewSizeOffset
        );
        const int bufferChunkCharCount = ::MultiByteToWideChar(
            codePage,
            0,
            viewChunkAddress,
            viewChunkSize,
            nullptr,
            0
        );
        THROW_LAST_ERROR_IF(bufferChunkCharCount == 0);
        viewSizeOffset += viewChunkSize;
        bufferCharCount += bufferChunkCharCount;
    }

    std::basic_string<BufferCharT> buffer(bufferCharCount, 0);

    // Convert to UTF-16
    size_t bufferCharCountOffset = 0;
    for (size_t viewSizeOffset = 0; viewSizeOffset < view.size();) {
        const LPCCH viewChunkAddress = getViewChunkAddress(
            view,
            viewSizeOffset
        );
        const int viewChunkSize = getViewChunkSize(
            view,
            viewSizeOffset
        );
        const auto bufferChunkAddress = reinterpret_cast<LPWSTR>(
            reinterpret_cast<uint16_t*>(buffer.data()) + bufferCharCountOffset
        );
        const auto bufferChunkCharCount = static_cast<int>((std::min)(
            static_cast<size_t>((std::numeric_limits<int>::max)()),
            buffer.size() - bufferCharCountOffset
        ));
        const int charsWritten = ::MultiByteToWideChar(
            codePage,
            0,
            viewChunkAddress,
            viewChunkSize,
            bufferChunkAddress,
            bufferChunkCharCount
        );
        THROW_LAST_ERROR_IF(charsWritten == 0);
        viewSizeOffset += viewChunkSize;
        bufferCharCountOffset += charsWritten;
    }

    return buffer;
}

template <typename ViewCharT> requires AChar<ViewCharT>
const auto getAViewChunkAddress = [](
    const std::basic_string_view<ViewCharT> view,
    const size_t viewSizeOffset) -> LPCCH {
    return reinterpret_cast<LPCCH>(view.data() + viewSizeOffset);
};

template <typename ViewCharT> requires AChar<ViewCharT>
const auto getAViewChunkSize = [](
    const std::basic_string_view<ViewCharT> view,
    const size_t viewSizeOffset) -> int {
    // 1 to 2 byte to character conversion ratio worst case scenario
    constexpr int maxViewChunkSize = (std::numeric_limits<int>::max)() / 2;
    return static_cast<int>((std::min)(
        static_cast<size_t>(maxViewChunkSize),
        view.size() - viewSizeOffset
    ));
};

template <typename ViewCharT> requires U8Char<ViewCharT>
const auto getU8ViewChunkAddress = [](
    const std::basic_string_view<ViewCharT> view,
    const size_t viewSizeOffset) -> LPCCH {
    return reinterpret_cast<LPCCH>(view.data() + viewSizeOffset);
};

// Continuation bytes handling to avoid splitting characters
template <typename ViewCharT> requires U8Char<ViewCharT>
const auto getU8ViewChunkSize = [](
    const std::basic_string_view<ViewCharT> view,
    const size_t viewSizeOffset) -> int {
    // 1 to 1 byte to character conversion ratio worst case scenario
    constexpr int maxViewChunkSize = (std::numeric_limits<int>::max)();
    const auto isContinuationByte = [](const ViewCharT byte) -> bool {
        return (byte & 0xC0) == 0x80;
    };
    if (const size_t remainingSize = view.size() - viewSizeOffset;
        remainingSize > maxViewChunkSize) {
        const size_t last = viewSizeOffset + maxViewChunkSize - 1;
        int viewChunkSize = maxViewChunkSize;
        if (isContinuationByte(view[last + 1])) {
            for (uint8_t i = 0; i < 3; ++i) {
                --viewChunkSize;
                if (!isContinuationByte(view[last - i])) {
                    break;
                }
            }
        }
        return viewChunkSize;
    } else {
        return static_cast<int>(remainingSize);
    }
};

template <typename ViewCharT> requires U16Char<ViewCharT>
const auto getU16ViewChunkAddress = [](
    const std::basic_string_view<ViewCharT> view,
    const size_t viewCharOffset) -> LPCWCH {
    return reinterpret_cast<LPCWCH>(view.data() + viewCharOffset);
};

// Surrogate pair handling to avoid splitting characters
template <typename ViewCharT> requires U16Char<ViewCharT>
const auto getU16ViewChunkCharCount = [](
    const std::basic_string_view<ViewCharT> view,
    const size_t viewCharOffset) -> int {
    // 1 to 3 character to byte conversion ratio worst case scenario
    constexpr int maxViewChunkCharCount = (std::numeric_limits<int>::max)() / 3;
    if (const size_t remainingCharCount = view.size() - viewCharOffset;
        remainingCharCount > maxViewChunkCharCount) {
        const size_t last = viewCharOffset + maxViewChunkCharCount - 1;
        return IS_HIGH_SURROGATE(view[last])
            ? maxViewChunkCharCount - 1 : maxViewChunkCharCount;
    } else {
        return static_cast<int>(remainingCharCount);
    }
};
} // namespace

namespace utils {
std::u8string AToU8(const std::string_view view) {
    using ViewCharT = char;
    using BufferCharT = char16_t;
    return U16ToU8(::CodePageToU16<ViewCharT, BufferCharT>(
        CP_ACP,
        view,
        getAViewChunkAddress<ViewCharT>,
        getAViewChunkSize<ViewCharT>
    ));
}

std::u16string AToU16(const std::string_view view) {
    using ViewCharT = char;
    using BufferCharT = char16_t;
    return ::CodePageToU16<ViewCharT, BufferCharT>(
        CP_ACP,
        view,
        getAViewChunkAddress<ViewCharT>,
        getAViewChunkSize<ViewCharT>
    );
}

std::u16string U8ToU16(const std::string_view view) {
    using ViewCharT = char;
    using BufferCharT = char16_t;
    return ::CodePageToU16<ViewCharT, BufferCharT>(
        CP_UTF8,
        view,
        getU8ViewChunkAddress<ViewCharT>,
        getU8ViewChunkSize<ViewCharT>
    );
}

std::u16string U8ToU16(const std::u8string_view view) {
    using ViewCharT = char8_t;
    using BufferCharT = char16_t;
    return ::CodePageToU16<ViewCharT, BufferCharT>(
        CP_UTF8,
        view,
        getU8ViewChunkAddress<ViewCharT>,
        getU8ViewChunkSize<ViewCharT>
    );
}

std::u8string U16ToU8(const std::wstring_view view) {
    using ViewCharT = wchar_t;
    using BufferCharT = char8_t;
    return ::U16ToCodePage<ViewCharT, BufferCharT>(
        CP_UTF8,
        view,
        getU16ViewChunkAddress<ViewCharT>,
        getU16ViewChunkCharCount<ViewCharT>
    );
}

std::u8string U16ToU8(const std::u16string_view view) {
    using ViewCharT = char16_t;
    using BufferCharT = char8_t;
    return ::U16ToCodePage<ViewCharT, BufferCharT>(
        CP_UTF8,
        view,
        getU16ViewChunkAddress<ViewCharT>,
        getU16ViewChunkCharCount<ViewCharT>
    );
}
} // namespace utils
