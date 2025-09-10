module;

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

module plugin;

import :FpsUnlocker;
import :Helper;

import std;
import pwu;

namespace {
constexpr uintptr_t OFFSET_OS = 0x3F3480C;
constexpr uintptr_t OFFSET_CN = 0x3F3580C;
} // namespace

namespace z3lx::plugin {
FpsUnlocker::FpsUnlocker() noexcept
    : isEnabled { false }
    , isAutoThrottle { false }
    , targetFps { 60 }
    , targetFpsPtr { nullptr } {}

FpsUnlocker::~FpsUnlocker() noexcept = default;

void FpsUnlocker::Start() {
    const auto [module, region] = GetGameModuleContext();
    const uintptr_t offset = [region] {
        switch (region) {
        case GameRegion::OS: return OFFSET_OS;
        case GameRegion::CN: return OFFSET_CN;
        default: pwu::ThrowWin32Error(ERROR_NOT_SUPPORTED);
        }
    }();

    targetFpsPtr = reinterpret_cast<int*>(
        reinterpret_cast<uintptr_t>(module) + offset
    );
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
