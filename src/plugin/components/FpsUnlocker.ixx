export module plugin:FpsUnlocker;

import :ComponentBase;
import :WindowState;

namespace z3lx::plugin {
class FpsUnlocker final : public ComponentBase<
    FpsUnlocker,
    WindowState
> {
public:
    FpsUnlocker() noexcept;
    ~FpsUnlocker() noexcept;

    void Start();
    void Update() const noexcept;

    [[nodiscard]] bool IsEnabled() const noexcept;
    void Enable(bool enable) noexcept;

    [[nodiscard]] int GetTargetFps() const noexcept;
    void SetTargetFps(int targetFps) noexcept;

    [[nodiscard]] bool IsAutoThrottle() const noexcept;
    void AutoThrottle(bool autoThrottle) noexcept;

private:
    bool isEnabled;
    bool isAutoThrottle;
    int targetFps;
    int* targetFpsPtr;
};
} // namespace z3lx::plugin
