#include "util/win/Dialogue.hpp"

#include <wil/result.h>

#include <array>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>

#include <Windows.h>
#include <commdlg.h>

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

std::filesystem::path OpenFileDialogue(
    const std::optional<std::span<const Filter>> filters,
    const std::optional<std::wstring_view> initialPath,
    const std::optional<std::wstring_view> dialogueTitle) {
    std::wstring stringFilter {};
    if (filters) {
        size_t size = 0;
        for (const auto& [name, spec] : *filters) {
            size += name.size() + spec.size() + 2;
        }
        stringFilter.reserve(size);
        for (const auto& [name, spec] : *filters) {
            stringFilter.append(name).push_back('\0');
            stringFilter.append(spec).push_back('\0');
        }
    }

    std::array<wchar_t, MAX_PATH> buffer {};
    OPENFILENAMEW ofn {
        .lStructSize = sizeof(ofn),
        .lpstrFilter = filters ? stringFilter.data() : nullptr,
        .nFilterIndex = filters ? 1UL : 0UL,
        .lpstrFile = buffer.data(),
        .nMaxFile = static_cast<DWORD>(buffer.size()),
        .lpstrInitialDir = initialPath ? initialPath->data() : nullptr,
        .lpstrTitle = dialogueTitle ? dialogueTitle->data() : nullptr,
        .Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER
    };
    THROW_IF_WIN32_BOOL_FALSE(GetOpenFileNameW(&ofn));
    return std::filesystem::path { buffer.data() };
}
} // namespace z3lx::util
