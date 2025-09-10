export module util:Version;

import std;

export namespace z3lx::util {
class Version final {
public:
    explicit Version(
        std::uint16_t major = 0,
        std::uint16_t minor = 0,
        std::uint16_t patch = 0,
        std::uint16_t tweak = 0
    ) noexcept;
    explicit Version(std::string_view version);
    explicit Version(std::wstring_view version);
    ~Version() noexcept;

    [[nodiscard]] std::uint16_t GetMajor() const noexcept;
    [[nodiscard]] std::uint16_t GetMinor() const noexcept;
    [[nodiscard]] std::uint16_t GetPatch() const noexcept;
    [[nodiscard]] std::uint16_t GetTweak() const noexcept;
    [[nodiscard]] std::string ToString() const;

    std::strong_ordering operator<=>(const Version& other) const noexcept;

private:
    std::uint16_t major;
    std::uint16_t minor;
    std::uint16_t patch;
    std::uint16_t tweak;
};
} // namespace z3lx::util
