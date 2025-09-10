#include "util/Version.hpp"

#include <array>
#include <charconv>
#include <compare>
#include <cstdint>
#include <format>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

namespace {
template <typename CharT>
bool ToU16(const std::basic_string_view<CharT> str, uint16_t& value) {
    if constexpr (std::is_same_v<CharT, char>) {
        const char* begin = str.data();
        const char* end = str.data() + str.size();
        auto [ptr, ec] = std::from_chars(begin, end, value);
        return ec == std::errc {} && ptr == end;
    } else if constexpr (std::is_same_v<CharT, wchar_t>) {
        if (str.size() > 5) {
            return false;
        }
        std::array<char, 6> buffer {};
        for (size_t i = 0; i < str.size(); ++i) {
            if (str[i] < L'0' || str[i] > L'9') {
                return false;
            }
            buffer[i] = static_cast<char>(str[i]);
        }
        const char* begin = buffer.data();
        const char* end = buffer.data() + str.size();
        auto [ptr, ec] = std::from_chars(begin, end, value);
        return ec == std::errc {} && ptr == end;
    }
    return false;
}

template <typename CharT>
void Parse(
    std::basic_string_view<CharT> version,
    uint16_t& major,
    uint16_t& minor,
    uint16_t& patch,
    uint16_t& tweak) {
    if (version.empty()) {
        throw std::invalid_argument { "Empty version string" };
    }

    const std::array segments { &major, &minor, &patch, &tweak };

    size_t idx = 0;
    for (std::ranges::split_view splits { version, '.' };
        auto split : splits) {
        if (idx >= segments.size()) {
            throw std::invalid_argument {
                "Too many segments in version string"
            };
        }

        const std::basic_string_view segment { split.begin(), split.end() };
        if (segment.empty()) {
            throw std::invalid_argument {
                "Empty segment in version string"
            };
        }

        if (!ToU16(segment, *segments[idx])) {
            throw std::invalid_argument {
                "Invalid segment in version string"
            };
        }

        ++idx;
    }
}
} // namespace

namespace z3lx::util {
Version::Version(
    const uint16_t major,
    const uint16_t minor,
    const uint16_t patch,
    const uint16_t tweak) noexcept
    : major { major }, minor { minor }, patch { patch }, tweak { tweak } {}

Version::Version(const std::string_view version)
    : major { 0 }, minor { 0 }, patch { 0 }, tweak { 0 } {
    Parse(version, major, minor, patch, tweak);
}

Version::Version(const std::wstring_view version)
    : major { 0 }, minor { 0 }, patch { 0 }, tweak { 0 } {
    Parse(version, major, minor, patch, tweak);
}

Version::~Version() noexcept = default;

uint16_t Version::GetMajor() const noexcept {
    return major;
}

uint16_t Version::GetMinor() const noexcept {
    return minor;
}

uint16_t Version::GetPatch() const noexcept {
    return patch;
}

uint16_t Version::GetTweak() const noexcept {
    return tweak;
}

std::string Version::ToString() const {
    return std::format("{}.{}.{}.{}", major, minor, patch, tweak);
}

std::strong_ordering Version::operator<=>(
    const Version& other) const noexcept = default;
} // namespace z3lx::util
