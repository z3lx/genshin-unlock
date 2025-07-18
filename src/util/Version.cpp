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

namespace z3lx::util {
Version::Version(
    const uint16_t major,
    const uint16_t minor,
    const uint16_t patch,
    const uint16_t tweak) noexcept
    : major { major }, minor { minor }, patch { patch }, tweak { tweak } {}

Version::Version(const std::string_view version)
    : major { 0 }, minor { 0 }, patch { 0 }, tweak { 0 } {
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

        const std::string_view segment { split.begin(), split.end() };
        if (segment.empty()) {
            throw std::invalid_argument {
                "Empty segment in version string"
            };
        }

        const char* begin = segment.data();
        const char* end = segment.data() + segment.size();
        if (const auto [ptr, ec] = std::from_chars(begin, end, *segments[idx]);
            ec != std::errc() || ptr != end) {
            throw std::invalid_argument {
                "Invalid segment in version string"
            };
        }

        ++idx;
    }
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
