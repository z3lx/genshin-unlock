#include "plugin/components/FovUnlocker.hpp"
#include "util/ExponentialFilter.hpp"
#include "util/MinHook.hpp"

#include <wil/result.h>

#include <bit>
#include <cmath>
#include <cstdint>
#include <mutex>

#include <Windows.h>

namespace {
constexpr uintptr_t OFFSET_GL = 0x1066530;
constexpr uintptr_t OFFSET_CN = 0x1068530;

void HkSetFieldOfView(void* instance, float value) noexcept;

std::mutex mutex {};
std::optional<z3lx::util::MinHook<void, void*, float>> minhook {};
z3lx::util::ExponentialFilter<float> filter {};

bool isHooked = false;
bool isEnabled = false;
bool isEnabledOnce = false;
int targetFov = 45;

int setFovCount = 0;
void* previousInstance = nullptr;
float previousFov = 45.0f;
bool isPreviousFov = false;
} // namespace

namespace z3lx::plugin {
FovUnlocker::FovUnlocker() noexcept = default;

FovUnlocker::~FovUnlocker() noexcept {
    std::lock_guard lock { mutex };
    minhook.reset();
}

void FovUnlocker::Start() {
    bool global = true;
    auto module = reinterpret_cast<uintptr_t>(
        GetModuleHandleA("GenshinImpact.exe")
    );

    if (!module) {
        global = false;
        module = reinterpret_cast<uintptr_t>(
            GetModuleHandleA("YuanShen.exe")
        );
        THROW_LAST_ERROR_IF(!module);
    }

    const uintptr_t offset = global ? OFFSET_GL : OFFSET_CN;
    const auto target = reinterpret_cast<void*>(module + offset);
    const auto detour = reinterpret_cast<void*>(HkSetFieldOfView);

    std::lock_guard lock { mutex };
    minhook = util::MinHook<void, void*, float> { target, detour };
}

void FovUnlocker::Update() {
    const auto& cursor = GetComponent<CursorState>();
    const auto& window = GetComponent<WindowState>();
    Hook(window.IsFocused() && !cursor.IsVisible());
}

bool FovUnlocker::IsHooked() const noexcept {
    return isHooked;
}

void FovUnlocker::Hook(const bool hook) {
    if (hook == isHooked) {
        return;
    }
    std::lock_guard lock { mutex };
    if (hook) {
        minhook->Enable(true);
        isEnabledOnce = true;
    } else {
        // minhook->Enable(false);
    }
    isHooked = hook;
}

bool FovUnlocker::IsEnabled() const noexcept {
    return isEnabled;
}

void FovUnlocker::Enable(const bool enable) noexcept {
    isEnabled = enable;
}

int FovUnlocker::GetTargetFov() const noexcept {
    return targetFov;
}

void FovUnlocker::SetTargetFov(const int targetFov) noexcept {
    ::targetFov = targetFov;
}

float FovUnlocker::GetSmoothing() const noexcept {
    return filter.GetTimeConstant();
}

void FovUnlocker::SetSmoothing(const float smoothing) noexcept {
    filter.SetTimeConstant(smoothing);
}
} // namespace z3lx::plugin

namespace {
void HkSetFieldOfView(void* instance, float value) noexcept try {
    std::lock_guard lock { mutex };
    if (!minhook.has_value()) {
        return;
    }

    ++setFovCount;
    if (const bool isDefaultFov = value == 45.0f;
        instance == previousInstance &&
        (value == previousFov || isDefaultFov)) {
        if (isDefaultFov) {
            previousInstance = instance;
            previousFov = value;
        }

        if (setFovCount > 8) {
            filter.SetInitialValue(value);
        }
        setFovCount = 0;

        if (isEnabledOnce) {
            isEnabledOnce = false;
            filter.Update(value);
        }
        const float target = (isHooked && isEnabled) ?
            static_cast<float>(targetFov) : previousFov;
        const float filtered = filter.Update(target);

        if ((isHooked && isEnabled) || !isPreviousFov) {
            isPreviousFov = std::abs(previousFov - filtered) < 0.1f;
            value = filtered;
        } else if (!isHooked) {
            isPreviousFov = false;
            minhook->Enable(false);
        }
    } else {
        const auto rep = std::bit_cast<std::uint32_t>(value);
        value = std::bit_cast<float>(rep + 1); // marker value
        previousInstance = instance;
        previousFov = value;
    }

    minhook->CallOriginal(instance, value);
} catch (...) {
    // Should never happen
    minhook->CallOriginal(instance, value);
}
} // namespace
