#pragma once

#include <string>
#include <string_view>

namespace z3lx::util {
std::u8string AToU8(std::string_view view);
std::u16string AToU16(std::string_view view);
std::u16string U8ToU16(std::string_view view);
std::u16string U8ToU16(std::u8string_view view);
std::u8string U16ToU8(std::wstring_view view);
std::u8string U16ToU8(std::u16string_view view);
} // namespace z3lx::util
