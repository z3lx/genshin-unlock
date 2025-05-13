#pragma once

#include <chrono>
#include <type_traits>

#define EXPONENTIALFILTER_TEMPLATE                                              \
    template <typename Real, typename Clock = std::chrono::steady_clock>        \
    requires std::is_floating_point_v<Real>

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

    Real TimeConstant() const noexcept;
    void TimeConstant(Real value) noexcept;
    Real InitialValue() const noexcept;
    void InitialValue(Real value) noexcept;
    Real Update(Real value) noexcept;

private:
    Real timeConstant;
    Real previousFilteredValue;
    std::chrono::time_point<Clock> previousTime;
};

#include "utils/ExponentialFilterInl.hpp"
