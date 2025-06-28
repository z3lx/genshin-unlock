#include "plugin/Config.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {
constexpr auto ENABLED = "enabled";
constexpr auto FOV = "fov";
constexpr auto FOV_PRESETS = "fov_presets";
constexpr auto SMOOTHING = "smoothing";
constexpr auto ENABLE_KEY = "enable_key";
constexpr auto NEXT_KEY = "next_key";
constexpr auto PREV_KEY = "prev_key";
} // namespace

namespace z3lx::gfu {
void Config::Serialize(std::string& buffer) {
    const nlohmann::ordered_json j {
        { ENABLED, enabled },
        { FOV, fov },
        { FOV_PRESETS, fovPresets },
        { SMOOTHING, smoothing },
        { ENABLE_KEY, enableKey },
        { NEXT_KEY, nextKey },
        { PREV_KEY, prevKey }
    };
    buffer = j.dump(4);
}

void Config::Deserialize(const std::string& buffer) {
    nlohmann::json j { nlohmann::json::parse(buffer) };

    const auto tryGetTo = [&j]<typename ValueT, typename Callable>(
        std::string_view key, ValueT& value, Callable condition) {
        ValueT parsedValue {};
        j.at(key).get_to(parsedValue);
        if (condition(parsedValue)) {
            value = std::move(parsedValue);
            return;
        }
        throw std::runtime_error {
            "Invalid value for key '" + std::string { key } + "': " +
            "condition is not satisfied"
        };
    };

    const auto isAlwaysValid = [](const auto&) { return true; };
    const auto isValidFov = [](const uint8_t fov) noexcept {
        return fov > 0 && fov < 180;
    };
    const auto isValidFovPresets = [isValidFov](
        std::vector<uint8_t>& fovPresets) noexcept {
        const bool valid = !fovPresets.empty() &&
            std::ranges::all_of(fovPresets, isValidFov);
        if (valid) {
            std::ranges::sort(fovPresets);
            const auto last = std::ranges::unique(fovPresets).begin();
            fovPresets.erase(last, fovPresets.end());
        }
        return valid;
    };
    const auto isValidSmoothing = [](const float smoothing) noexcept {
        return smoothing >= 0.0f && smoothing <= 1.0f;
    };

    tryGetTo(ENABLED, enabled, isAlwaysValid);
    tryGetTo(FOV, fov, isValidFov);
    tryGetTo(FOV_PRESETS, fovPresets, isValidFovPresets);
    tryGetTo(SMOOTHING, smoothing, isValidSmoothing);
    tryGetTo(ENABLE_KEY, enableKey, isAlwaysValid);
    tryGetTo(NEXT_KEY, nextKey, isAlwaysValid);
    tryGetTo(PREV_KEY, prevKey, isAlwaysValid);
}
} // namespace z3lx::gfu
