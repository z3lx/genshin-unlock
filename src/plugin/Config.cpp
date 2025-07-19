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
    .append_arrays = true,
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
struct glz::meta<z3lx::plugin::Config> {
    using T = z3lx::plugin::Config;

    static constexpr auto fpsOverrideConstraintCondition = [](
        const T&, const int fps) -> bool {
        return fps >= -1;
    };
    static constexpr auto fpsOverrideConstraint = read_constraint<
        &T::targetFps, fpsOverrideConstraintCondition,
        "Target FPS must be -1 or greater"
    >;

    static constexpr auto isValidFov = [](const uint8_t fov) -> bool {
        return fov > 0 && fov < 180;
    };

    static constexpr auto fovOverrideConstraintCondition = [](
        const T&, const int fov) -> bool {
        return isValidFov(fov);
    };
    static constexpr auto fovOverrideConstraint = read_constraint<
        &T::targetFov, fovOverrideConstraintCondition,
        "Target FOV must be between 1 and 179 degrees"
    >;

    static constexpr auto fovPresetsConstraintCondition = [](
        const T&, std::vector<int>& fovPresets) -> bool {
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
        "FOV presets must be between 1 and 179 degrees"
    >;

    static constexpr auto fovSmoothingConstraintCondition = [](
        const T&, const float smoothing) -> bool {
        return smoothing >= 0.0f && smoothing <= 1.0f;
    };
    static constexpr auto fovSmoothingConstraint = read_constraint<
        &T::fovSmoothing, fovSmoothingConstraintCondition,
        "FOV smoothing must be between 0.0 and 1.0"
    >;

    static constexpr auto value = object(
        &T::unlockFps,
        "targetFps", fpsOverrideConstraint,
        &T::autoThrottle,
        &T::unlockFov,
        "targetFov", fovOverrideConstraint,
        "fovPresets", fovPresetsConstraint,
        "fovSmoothing", fovSmoothingConstraint,
        &T::unlockFovKey,
        &T::nextFovPresetKey,
        &T::prevFovPresetKey
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

namespace z3lx::plugin {
void Config::Serialize(std::vector<uint8_t>& buffer) {
    glz::ex::write<opts>(*this, buffer);
}

void Config::Deserialize(const std::vector<uint8_t>& buffer) {
    glz::ex::read<opts>(*this, buffer);
}
} // namespace z3lx::plugin
