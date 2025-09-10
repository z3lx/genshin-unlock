export module plugin:WindowState;

import std;

import :ComponentBase;

namespace z3lx::plugin {
class WindowState final : public ComponentBase<WindowState> {
public:
    WindowState() noexcept;
    ~WindowState() noexcept;

    void Update() noexcept;

    [[nodiscard]] bool IsFocused() const noexcept;

private:
    std::optional<bool> isFocused;
};
} // namespace z3lx::plugin
