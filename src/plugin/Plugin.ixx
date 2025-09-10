export module plugin:Plugin;

import :ComponentBase;
import :Config;
import :FovUnlocker;
import :FpsUnlocker;
import :PersistentObject;
import :VirtualKeyState;
import :ComponentBase;

import std;

export namespace z3lx::plugin {
class Plugin final : public ComponentBase<
    Plugin,
    FpsUnlocker,
    FovUnlocker,
    PersistentObject<Config>,
    VirtualKeyState
> {
public:
    explicit Plugin(std::filesystem::path configFilePath);
    ~Plugin() noexcept;

    void Start();
    void Update();

private:
    std::filesystem::path configFilePath;
};
} // namespace z3lx::plugin
