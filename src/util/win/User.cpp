#include "util/win/User.hpp"

#include <wil/result.h>

#include <string_view>
#include <type_traits>

#include <Windows.h>

namespace {
template <typename StringView>
z3lx::util::MessageBoxResult ShowMessageBox(
    const StringView title,
    const StringView message,
    const z3lx::util::MessageBoxIcon icon,
    const z3lx::util::MessageBoxButton button,
    const z3lx::util::MessageBoxDefaultButton defaultButton) {
    constexpr auto func = [] {
        using CharT = typename StringView::value_type;
        if constexpr (std::is_same_v<CharT, char>) {
            return MessageBoxA;
        } else if constexpr (std::is_same_v<CharT, wchar_t>) {
            return MessageBoxW;
        }
    }();
    int result = func(
        nullptr,
        message.data(),
        title.data(),
        static_cast<UINT>(icon) |
        static_cast<UINT>(button) |
        static_cast<UINT>(defaultButton)
    );
    THROW_LAST_ERROR_IF(result == 0);
    return static_cast<z3lx::util::MessageBoxResult>(result);
}
} // namespace

namespace z3lx::util {
MessageBoxResult ShowMessageBox(
    const std::string_view title,
    const std::string_view message,
    const MessageBoxIcon icon,
    const MessageBoxButton button,
    const MessageBoxDefaultButton defaultButton) {
    return ::ShowMessageBox(
        title,
        message,
        icon,
        button,
        defaultButton
    );
}

MessageBoxResult ShowMessageBox(
    const std::wstring_view title,
    const std::wstring_view message,
    const MessageBoxIcon icon,
    const MessageBoxButton button,
    const MessageBoxDefaultButton defaultButton) {
    return ::ShowMessageBox(
        title,
        message,
        icon,
        button,
        defaultButton
    );
}
} // namespace z3lx::util
