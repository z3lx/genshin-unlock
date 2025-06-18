#include "plugin/components/Unlocker.hpp"
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
std::optional<z3lx::util::MinHook<void, void*, float>> hook {};
z3lx::util::ExponentialFilter<float> filter {};

bool isHooked = false;
bool isEnabled = false;
bool isEnabledOnce = false;
int overrideFov = 45;

int setFovCount = 0;
void* previousInstance = nullptr;
float previousFov = 45.0f;
bool isPreviousFov = false;
} // namespace

namespace z3lx::gfu {
Unlocker::Unlocker() noexcept = default;

Unlocker::~Unlocker() noexcept {
    std::lock_guard lock { mutex };
    hook.reset();
}

void Unlocker::Start() {
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
    hook = util::MinHook<void, void*, float> { target, detour };
}

bool Unlocker::Hooked() const noexcept {
    return isHooked;
}

void Unlocker::Hooked(const bool value) {
    std::lock_guard lock { mutex };
    isHooked = value;
    if (value) {
        hook->Enable();
        isEnabledOnce = true;
    } else {
        // hook->Disable();
    }
}

bool Unlocker::Enabled() const noexcept {
    return isEnabled;
}

void Unlocker::Enabled(const bool value) noexcept {
    isEnabled = value;
}

int Unlocker::FieldOfView() const noexcept {
    return overrideFov;
}

void Unlocker::FieldOfView(const int value) noexcept {
    overrideFov = value;
}

float Unlocker::Smoothing() const noexcept {
    return filter.TimeConstant();
}

void Unlocker::Smoothing(const float value) noexcept {
    filter.TimeConstant(value);
}
} // namespace z3lx::gfu

namespace {
void HkSetFieldOfView(void* instance, float value) noexcept try {
    std::lock_guard lock { mutex };
    if (!hook.has_value()) {
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
            filter.InitialValue(value);
        }
        setFovCount = 0;

        if (isEnabledOnce) {
            isEnabledOnce = false;
            filter.Update(value);
        }
        const float target = (isHooked && isEnabled) ?
            static_cast<float>(overrideFov) : previousFov;
        const float filtered = filter.Update(target);

        if ((isHooked && isEnabled) || !isPreviousFov) {
            isPreviousFov = std::abs(previousFov - filtered) < 0.1f;
            value = filtered;
        } else if (!isHooked) {
            isPreviousFov = false;
            hook->Disable();
        }
    } else {
        const auto rep = std::bit_cast<std::uint32_t>(value);
        value = std::bit_cast<float>(rep + 1); // marker value
        previousInstance = instance;
        previousFov = value;
    }

    hook->CallOriginal(instance, value);
} catch (...) {
    // Should never happen
    hook->CallOriginal(instance, value);
}
} // namespace
