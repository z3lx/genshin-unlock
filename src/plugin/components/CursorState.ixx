export module plugin:CursorState;

import :ComponentBase;

import std;

namespace z3lx::plugin {
class CursorState final : public ComponentBase<CursorState> {
public:
    CursorState() noexcept;
    ~CursorState() noexcept;

    void Update() noexcept;

    [[nodiscard]] bool IsVisible() const noexcept;

private:
    std::optional<bool> isVisible;
};
} // namespace z3lx::plugin
