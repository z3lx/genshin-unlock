#include "util/win/User.hpp"

#include <wil/result.h>

#include <algorithm>
#include <chrono>
#include <thread>
#include <tuple>
#include <vector>

#include <Windows.h>

namespace z3lx::util {
std::vector<HWND> GetCurrentProcessWindows() {
    return GetProcessWindows(GetCurrentProcessId());
}

std::vector<HWND> GetProcessWindows(const DWORD processId) {
    std::vector<HWND> windows {};
    std::tuple params = std::tie(processId, windows);

    const auto callback = [](const HWND hwnd, const LPARAM lParam) -> BOOL {
        auto& [targetProcessId, targetWindows] = *reinterpret_cast<
            std::tuple<DWORD&, std::vector<HWND>&>*>(lParam);

        DWORD processId {};
        if (!::GetWindowThreadProcessId(hwnd, &processId)) {
            return FALSE;
        }
        if (processId == targetProcessId &&
            !std::ranges::contains(targetWindows, hwnd)) {
            targetWindows.push_back(hwnd);
        }
        return TRUE;
    };

    const auto enumerateWindows = [&callback, &params]() {
        THROW_IF_WIN32_BOOL_FALSE(::EnumWindows(
            callback, reinterpret_cast<LPARAM>(&params)
        ));
    };
    while (windows.empty()) {
        enumerateWindows();
        std::this_thread::sleep_for(std::chrono::seconds { 1 });
    }
    enumerateWindows();
    return windows;
}
} // namespace z3lx::util
