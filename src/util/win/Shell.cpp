#include "util/win/Shell.hpp"

#include <wil/result.h>

#include <string_view>
#include <type_traits>

#include <Windows.h>
#include <shellapi.h>

#undef ShellExecute

namespace {
template <typename StringView>
void ShellExecute(const StringView operation, const StringView file) {
    constexpr auto func = [] {
        using CharT = typename StringView::value_type;
        if constexpr (std::is_same_v<CharT, char>) {
            return ShellExecuteA;
        } else if constexpr (std::is_same_v<CharT, wchar_t>) {
            return ShellExecuteW;
        }
    }();
    const auto code = reinterpret_cast<INT_PTR>(func(
        nullptr,
        operation.data(),
        file.data(),
        nullptr,
        nullptr,
        SW_SHOWNORMAL
    ));
    THROW_LAST_ERROR_IF(code <= 32);
}
} // namespace

namespace z3lx::util {
void OpenUrl(const std::string_view url) {
    constexpr std::string_view operation { "open" };
    ShellExecute(operation, url);
}

void OpenUrl(const std::wstring_view url) {
    constexpr std::wstring_view operation { L"open" };
    ShellExecute(operation, url);
}
} // namespace z3lx::util
