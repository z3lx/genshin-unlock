module;

#define COMPONENTBASE_TEMPLATE                                                     \
    template <typename Derived, typename... Components>

#define COMPONENTBASE                                                              \
    z3lx::plugin::ComponentBase<Derived, Components...>

export module plugin:ComponentBase;

import pwu;
import std;

namespace z3lx::plugin {
template <typename Component>
struct ComponentStorage;

COMPONENTBASE_TEMPLATE
class ComponentBase : ComponentStorage<Components>... {
    template <typename D, typename... Cs>
    friend class ComponentBase;

public:
    ComponentBase();
    ~ComponentBase() noexcept;
    ComponentBase(const ComponentBase&) = delete;
    ComponentBase(ComponentBase&&) = delete;

    [[noreturn]] void Run(std::uint16_t frequency = 60);

protected:
    constexpr static void Start();
    constexpr static void Update();

    template <typename Component>
    [[nodiscard]] Component& GetComponent() noexcept;

    template <typename Component>
    [[nodiscard]] const Component& GetComponent() const noexcept;

private:
    void StartComponent();
    void UpdateComponent();
};
} // namespace z3lx::plugin

namespace z3lx::plugin {
template <typename Component>
struct ComponentStorage {
    Component value {};
};

COMPONENTBASE_TEMPLATE
COMPONENTBASE::ComponentBase() = default;

COMPONENTBASE_TEMPLATE
COMPONENTBASE::~ComponentBase() noexcept = default;

COMPONENTBASE_TEMPLATE
void COMPONENTBASE::Run(const std::uint16_t frequency) try {
    auto derived = static_cast<Derived*>(this);
    derived->StartComponent();
    while (true) {
        derived->UpdateComponent();
        if (frequency != 0) {
            using Milliseconds = std::chrono::duration<double, std::milli>;
            const Milliseconds duration { 1000.0 / frequency };
            std::this_thread::sleep_for(duration);
        }
    }
} catch (...) {
    pwu::ThrowCaughtTraced();
}

COMPONENTBASE_TEMPLATE
constexpr void COMPONENTBASE::Start() {}

COMPONENTBASE_TEMPLATE
constexpr void COMPONENTBASE::Update() {}

COMPONENTBASE_TEMPLATE
template <typename Component>
Component& COMPONENTBASE::GetComponent() noexcept {
    return ComponentStorage<Component>::value;
}

COMPONENTBASE_TEMPLATE
template <typename Component>
const Component& COMPONENTBASE::GetComponent() const noexcept {
    return ComponentStorage<Component>::value;
}

COMPONENTBASE_TEMPLATE
void COMPONENTBASE::StartComponent() try {
    (GetComponent<Components>().StartComponent(), ...);
    static_cast<Derived*>(this)->Start();
} catch (...) {
    pwu::ThrowCaughtTraced();
}

COMPONENTBASE_TEMPLATE
void COMPONENTBASE::UpdateComponent() try {
    (GetComponent<Components>().UpdateComponent(), ...);
    static_cast<Derived*>(this)->Update();
} catch (...) {
    pwu::ThrowCaughtTraced();
}
} // namespace z3lx::plugin
