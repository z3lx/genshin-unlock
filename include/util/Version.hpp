#pragma once

#include <compare>
#include <cstdint>
#include <string>
#include <string_view>

namespace z3lx::util {
class Version final {
public:
    explicit Version(
        uint16_t major = 0,
        uint16_t minor = 0,
        uint16_t patch = 0,
        uint16_t tweak = 0
    ) noexcept;
    explicit Version(std::string_view version);
    ~Version() noexcept;

    [[nodiscard]] uint16_t GetMajor() const noexcept;
    [[nodiscard]] uint16_t GetMinor() const noexcept;
    [[nodiscard]] uint16_t GetPatch() const noexcept;
    [[nodiscard]] uint16_t GetTweak() const noexcept;
    [[nodiscard]] std::string ToString() const;

    std::strong_ordering operator<=>(const Version& other) const noexcept;

private:
    uint16_t major;
    uint16_t minor;
    uint16_t patch;
    uint16_t tweak;
};
} // namespace z3lx::util
