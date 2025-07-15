#include "plugin/components/FpsUnlocker.hpp"

#include <wil/result.h>

#include <Windows.h>

namespace {
constexpr uintptr_t OFFSET_GL = 0x3E81644;
constexpr uintptr_t OFFSET_CN = 0x3E83644;
} // namespace

namespace z3lx::plugin {
FpsUnlocker::FpsUnlocker() noexcept
    : isEnabled { false }
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
    if (isEnabled && targetFpsPtr) {
        *targetFpsPtr = targetFps;
    }
}

bool FpsUnlocker::IsEnabled() const noexcept {
    return isEnabled;
}

void FpsUnlocker::Enable(const bool enable) noexcept {
    isEnabled = enable;
}

int FpsUnlocker::GetTargetFps() const noexcept {
    return targetFps;
}

void FpsUnlocker::SetTargetFps(const int targetFps) noexcept {
    this->targetFps = targetFps;
}
} // namespace z3lx::plugin
