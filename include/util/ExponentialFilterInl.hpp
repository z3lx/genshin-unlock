#pragma once

#include "util/ExponentialFilter.hpp"

#include <chrono>
#include <cmath>

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
Real ExponentialFilter<Real, Clock>::TimeConstant() const noexcept {
    return timeConstant;
}

EXPONENTIALFILTER_TEMPLATE
void ExponentialFilter<Real, Clock>::TimeConstant(const Real value) noexcept {
    timeConstant = value;
}

EXPONENTIALFILTER_TEMPLATE
Real ExponentialFilter<Real, Clock>::InitialValue() const noexcept {
    return previousFilteredValue;
}

EXPONENTIALFILTER_TEMPLATE
void ExponentialFilter<Real, Clock>::InitialValue(const Real value) noexcept {
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

#undef EXPONENTIALFILTER_TEMPLATE
