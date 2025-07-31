#include "loader/Config.hpp"

#include <glaze/glaze.hpp>
#include <glaze/glaze_exceptions.hpp>

#include <cstdint>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace {
constexpr glz::opts opts {
    .null_terminated = false,
    .comments = false,
    .error_on_unknown_keys = true,
    .skip_null_members = false,
    .prettify = true,
    .minified = false,
    .indentation_char = ' ',
    .indentation_width = 4,
    .new_lines_in_arrays = true,
    .append_arrays = false,
    .error_on_missing_keys = true,
    .error_on_const_read = true,
    .bools_as_numbers = false,
    .quoted_num = false,
    .number = false,
    .raw = false,
    .raw_string = false,
    .structs_as_arrays = false,
    .partial_read = false
};
} // namespace

template <>
struct glz::meta<z3lx::loader::DisplayMode> {
    using enum z3lx::loader::DisplayMode;
    static constexpr auto value = enumerate(
        Windowed,
        Fullscreen,
        Borderless
    );
};

namespace z3lx::loader {
void Config::Serialize(std::vector<uint8_t>& buffer) {
    glz::ex::write<opts>(*this, buffer);
}

void Config::Deserialize(const std::vector<uint8_t>& buffer) {
    glz::ex::read<opts>(*this, buffer);
}
} // namespace z3lx::loader
