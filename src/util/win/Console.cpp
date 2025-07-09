#include "util/win/Console.hpp"

#include <iostream>

#include <Windows.h>
#include <conio.h>

namespace z3lx::util {
void Pause() noexcept {
    std::cout << "Press any key to continue..." << std::endl;
    _getch();
}
} // namespace z3lx::util
