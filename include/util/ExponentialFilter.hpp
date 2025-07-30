#pragma once

#include <chrono>
#include <type_traits>

#define EXPONENTIALFILTER_TEMPLATE                                              \
    template <typename Real, typename Clock = std::chrono::steady_clock>        \
    requires std::is_floating_point_v<Real>

namespace z3lx::util {
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

#include "util/ExponentialFilterInl.hpp"
