#include "plugin/Config.hpp"
#include "util/win/VirtualKey.hpp"

#include <glaze/glaze.hpp>
#include <glaze/glaze_exceptions.hpp>

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {
constexpr glz::opts opts {
    .null_terminated = false,
    .comments = false,
    .error_on_unknown_keys = true,
    .skip_null_members = false,
    .prettify = true,
    .minified = false,
    .indentation_char = ' ',
    .indentation_width = 4,
    .new_lines_in_arrays = true,
    .append_arrays = false,
    .error_on_missing_keys = true,
    .error_on_const_read = true,
    .bools_as_numbers = false,
    .quoted_num = false,
    .number = false,
    .raw = false,
    .raw_string = false,
    .structs_as_arrays = false,
    .partial_read = false
};
} // namespace

template <>
struct glz::meta<z3lx::gfu::Config> {
    using T = z3lx::gfu::Config;

    static constexpr auto isValidFov = [](const uint8_t fov) {
        return fov > 0 && fov < 180;
    };

    static constexpr auto fovConstraintCondition = [](
        const T&, const uint8_t fov) {
        return isValidFov(fov);
    };
    static constexpr auto fovConstraint = read_constraint<
        &T::fov, fovConstraintCondition,
        "FOV must be between 1 and 179 degrees"
    >;

    static constexpr auto fovPresetsConstraintCondition = [](
        const T&, std::vector<uint8_t>& fovPresets) {
        if (!fovPresets.empty() &&
            std::ranges::all_of(fovPresets, isValidFov)) {
            std::ranges::sort(fovPresets);
            const auto last = std::ranges::unique(fovPresets).begin();
            fovPresets.erase(last, fovPresets.end());
            return true;
        }
        return false;
    };
    static constexpr auto fovPresetsConstraint = read_constraint<
        &T::fovPresets, fovPresetsConstraintCondition,
        "All FOV presets must be between 1 and 179 degrees"
    >;

    static constexpr auto smoothingConstraintCondition = [](
        const T&, const float smoothing) {
        return smoothing >= 0.0f && smoothing <= 1.0f;
    };
    static constexpr auto smoothingConstraint = read_constraint<
        &T::smoothing, smoothingConstraintCondition,
        "Smoothing must be between 0.0 and 1.0"
    >;

    static constexpr auto value = object(
        "enabled", &T::enabled,
        "fov", fovConstraint,
        "fovPresets", fovPresetsConstraint,
        "smoothing", smoothingConstraint,
        "enableKey", &T::enableKey,
        "nextKey", &T::nextKey,
        "prevKey", &T::prevKey
    );
};

template <>
struct glz::meta<z3lx::util::VirtualKey> {
    using enum z3lx::util::VirtualKey;
    static constexpr auto value = enumerate(
        LeftMouse,
        RightMouse,
        // Cancel,
        MiddleMouse,
        X1Mouse,
        X2Mouse,
        Backspace,
        Tab,
        Clear,
        Enter,
        Shift,
        Ctrl,
        Alt,
        Pause,
        CapsLock,
        // ImeKana,
        // ImeHangul,
        // ImeOn,
        // ImeJunja,
        // ImeFinal,
        // ImeHanja,
        // ImeKanji,
        // ImeOff,
        Esc,
        // ImeConvert,
        // ImeNonConvert,
        // ImeAccept,
        // ImeModeChange,
        Space,
        PageUp,
        PageDown,
        End,
        Home,
        LeftArrow,
        UpArrow,
        RightArrow,
        DownArrow,
        // Select,
        // Print,
        // Execute,
        PrintScreen,
        Insert,
        Delete,
        // Help,
        "0", D0,
        "1", D1,
        "2", D2,
        "3", D3,
        "4", D4,
        "5", D5,
        "6", D6,
        "7", D7,
        "8", D8,
        "9", D9,
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        LeftWindows,
        RightWindows,
        Apps,
        // Sleep,
        Numpad0,
        Numpad1,
        Numpad2,
        Numpad3,
        Numpad4,
        Numpad5,
        Numpad6,
        Numpad7,
        Numpad8,
        Numpad9,
        NumpadMultiply,
        NumpadAdd,
        NumpadSeparator,
        NumpadSubtract,
        NumpadDecimal,
        NumpadDivide,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        F16,
        F17,
        F18,
        F19,
        F20,
        F21,
        F22,
        F23,
        F24,
        NumLock,
        ScrollLock,
        LeftShift,
        RightShift,
        LeftCtrl,
        RightCtrl,
        LeftAlt,
        RightAlt,
        // BrowserBack,
        // BrowserForward,
        // BrowserRefresh,
        // BrowserStop,
        // BrowserSearch,
        // BrowserFavorites,
        // BrowserHome,
        // VolumeMute,
        // VolumeDown,
        // VolumeUp,
        // MediaNextTrack,
        // MediaPrevTrack,
        // MediaStop,
        // MediaPlayPause,
        // LaunchMail,
        // LaunchMediaSelect,
        // LaunchApp1,
        // LaunchApp2,
        Oem1,
        "Plus", OemPlus,
        "Comma", OemComma,
        "Minus", OemMinus,
        "Period", OemPeriod,
        Oem2,
        Oem3,
        Oem4,
        Oem5,
        Oem6,
        Oem7,
        Oem8,
        Oem102,
        // ImeProcess,
        // Packet,
        // Attn,
        // CrSel,
        // ExSel,
        // EraseEof,
        // Play,
        // Zoom,
        // NoName,
        // Pa1,
        OemClear
    );
};

namespace z3lx::gfu {
void Config::Serialize(std::vector<uint8_t>& buffer) {
    glz::ex::write<opts>(*this, buffer);
}

void Config::Deserialize(const std::vector<uint8_t>& buffer) {
    glz::ex::read<opts>(*this, buffer);
}
} // namespace z3lx::gfu
