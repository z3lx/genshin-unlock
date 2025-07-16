#include "plugin/components/FpsUnlocker.hpp"

#include <wil/result.h>

#include <algorithm>
#include <cstdint>

#include <Windows.h>

namespace {
constexpr uintptr_t OFFSET_GL = 0x3E81644;
constexpr uintptr_t OFFSET_CN = 0x3E83644;
} // namespace

namespace z3lx::plugin {
FpsUnlocker::FpsUnlocker() noexcept
    : isEnabled { false }
    , isAutoThrottle { false }
    , targetFps { 60 }
    , targetFpsPtr { nullptr } {}

FpsUnlocker::~FpsUnlocker() noexcept = default;

void FpsUnlocker::Start() {
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
    targetFpsPtr = reinterpret_cast<int*>(module + offset);
}

void FpsUnlocker::Update() const noexcept {
    if (!isEnabled) {
        return;
    }

    if (isAutoThrottle) {
        const auto& window = GetComponent<WindowState>();
        constexpr int idleTargetFps = 10;
        *targetFpsPtr = window.IsFocused()
            ? targetFps
            : (std::min)(targetFps, idleTargetFps);

        const DWORD priority = window.IsFocused()
            ? NORMAL_PRIORITY_CLASS : IDLE_PRIORITY_CLASS;
        SetPriorityClass(GetCurrentProcess(), priority);
    } else {
        *targetFpsPtr = targetFps;
    }
}

bool FpsUnlocker::IsEnabled() const noexcept {
    return isEnabled;
}

void FpsUnlocker::Enable(const bool enable) noexcept {
    isEnabled = enable;
}

bool FpsUnlocker::IsAutoThrottle() const noexcept {
    return isAutoThrottle;
}

void FpsUnlocker::AutoThrottle(const bool autoThrottle) noexcept {
    isAutoThrottle = autoThrottle;
}

int FpsUnlocker::GetTargetFps() const noexcept {
    return targetFps;
}

void FpsUnlocker::SetTargetFps(const int targetFps) noexcept {
    this->targetFps = targetFps;
}
} // namespace z3lx::plugin
