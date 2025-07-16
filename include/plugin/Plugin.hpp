#pragma once

#include "plugin/Config.hpp"
#include "plugin/components/FovUnlocker.hpp"
#include "plugin/components/FpsUnlocker.hpp"
#include "plugin/components/PersistentObject.hpp"
#include "plugin/components/VirtualKeyState.hpp"
#include "plugin/interfaces/IComponent.hpp"
#include "plugin/interfaces/IRunnable.hpp"

#include <filesystem>

namespace z3lx::plugin {
class Plugin final
    : public IComponent<
        Plugin,
        FpsUnlocker,
        FovUnlocker,
        PersistentObject<Config>,
        VirtualKeyState>
    , public IRunnable<Plugin> {
public:
    explicit Plugin(std::filesystem::path configFilePath);
    ~Plugin() noexcept;

    void Start();
    void Update();

private:
    std::filesystem::path configFilePath;
};
} // namespace z3lx::plugin
