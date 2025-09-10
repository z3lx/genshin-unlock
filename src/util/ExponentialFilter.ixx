module;

#define EXPONENTIALFILTER_TEMPLATE                                              \
    template <typename Real, typename Clock = std::chrono::steady_clock>        \
    requires std::is_floating_point_v<Real>

export module util:ExponentialFilter;

import std;

export namespace z3lx::util {
EXPONENTIALFILTER_TEMPLATE
class ExponentialFilter {
public:
    explicit ExponentialFilter(
        Real timeConstant = 0,
        Real initialValue = 0) noexcept;
    ~ExponentialFilter() noexcept;

    ExponentialFilter(const ExponentialFilter& other) noexcept;
    ExponentialFilter& operator=(const ExponentialFilter& other) noexcept;
    ExponentialFilter(ExponentialFilter&& other) noexcept;
    ExponentialFilter& operator=(ExponentialFilter&& other) noexcept;

    [[nodiscard]] Real GetTimeConstant() const noexcept;
    void SetTimeConstant(Real value) noexcept;
    [[nodiscard]] Real GetInitialValue() const noexcept;
    void SetInitialValue(Real value) noexcept;
    Real Update(Real value) noexcept;

private:
    Real timeConstant;
    Real previousFilteredValue;
    std::chrono::time_point<Clock> previousTime;
};
} // namespace z3lx::util

namespace z3lx::util {
EXPONENTIALFILTER_TEMPLATE
ExponentialFilter<Real, Clock>::ExponentialFilter(
    const Real timeConstant, const Real initialValue) noexcept
    : timeConstant { timeConstant }
    , previousFilteredValue { initialValue }
    , previousTime { Clock::now() } {}

EXPONENTIALFILTER_TEMPLATE
ExponentialFilter<Real, Clock>::~ExponentialFilter() noexcept = default;

EXPONENTIALFILTER_TEMPLATE
ExponentialFilter<Real, Clock>::ExponentialFilter(
    const ExponentialFilter& other) noexcept = default;

EXPONENTIALFILTER_TEMPLATE
ExponentialFilter<Real, Clock>&
ExponentialFilter<Real, Clock>::operator=(
    const ExponentialFilter& other) noexcept = default;

EXPONENTIALFILTER_TEMPLATE
ExponentialFilter<Real, Clock>::ExponentialFilter(
    ExponentialFilter&& other) noexcept = default;

EXPONENTIALFILTER_TEMPLATE
ExponentialFilter<Real, Clock>&
ExponentialFilter<Real, Clock>::operator=(
    ExponentialFilter&& other) noexcept = default;

EXPONENTIALFILTER_TEMPLATE
Real ExponentialFilter<Real, Clock>::GetTimeConstant() const noexcept {
    return timeConstant;
}

EXPONENTIALFILTER_TEMPLATE
void ExponentialFilter<Real, Clock>::SetTimeConstant(const Real value) noexcept {
    timeConstant = value;
}

EXPONENTIALFILTER_TEMPLATE
Real ExponentialFilter<Real, Clock>::GetInitialValue() const noexcept {
    return previousFilteredValue;
}

EXPONENTIALFILTER_TEMPLATE
void ExponentialFilter<Real, Clock>::SetInitialValue(const Real value) noexcept {
    previousFilteredValue = value;
    previousTime = Clock::now();
}

EXPONENTIALFILTER_TEMPLATE
Real ExponentialFilter<Real, Clock>::Update(const Real value) noexcept {
    // y(k) = alpha * y(k - 1) + (1 - alpha) * x(k)
    // alpha = exp(-T / tau)
    //
    // Where:
    // - x(k) is the raw input at time step k
    // - y(k) is the filtered output at time step k
    // - T is the time since the previous time step
    // - tau is the time constant

    const auto currentTime = Clock::now();
    using ToSecondsReal = std::chrono::duration<Real>;
    const auto deltaTime = std::chrono::duration_cast<ToSecondsReal>(
        currentTime - previousTime).count();
    previousTime = currentTime;

    if (timeConstant <= 0) {
        return value;
    }

    const auto alpha = std::exp(-deltaTime / timeConstant);
    previousFilteredValue = alpha * previousFilteredValue +
        (static_cast<Real>(1) - alpha) * value;
    return previousFilteredValue;
}
} // namespace z3lx::util
