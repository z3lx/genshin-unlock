#include "loader/Config.hpp"
#include "util/win/String.hpp"

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
struct glz::meta<z3lx::loader::Config> {
    using T = z3lx::loader::Config;

    static constexpr auto isValidFile = [](fs::path& file) {
        if (!fs::exists(file) || !fs::is_regular_file(file)) {
            return false;
        }
        file = fs::absolute(file.make_preferred());
        return true;
    };

    static constexpr auto gamePathConstraintCondition = [](
        const T&, fs::path& gamePath) -> bool {
        return isValidFile(gamePath) && gamePath.extension() == ".exe";
    };
    static constexpr auto gamePathConstraint = read_constraint<
        &T::gamePath, gamePathConstraintCondition,
        "Game path must be a valid file"
    >;

    static constexpr auto readGameArgs = [](
        T& config, const std::string& input) -> void {
        z3lx::util::U8ToU16(input, config.gameArgs);
    };
    static constexpr auto writeGameArgs = [](
        const T& config) -> std::string {
        std::string output {};
        z3lx::util::U16ToU8(config.gameArgs, output);
        return output;
    };
    static constexpr auto gameArgsHandler = custom<
        readGameArgs, writeGameArgs
    >;

    static constexpr auto dllPathsConstraintCondition = [](
        const T&, std::vector<fs::path>& dllPaths) -> bool {
        for (auto& dllPath : dllPaths) {
            if (!isValidFile(dllPath) || dllPath.extension() != ".dll") {
                return false;
            }
        }
        return true;
    };
    static constexpr auto dllPathsConstraint = read_constraint<
        &T::dllPaths, dllPathsConstraintCondition,
        "Dll paths must be valid files"
    >;

    static constexpr auto value = object(
        "gamePath", gamePathConstraint,
        "gameArgs", gameArgsHandler,
        "dllPaths", dllPathsConstraint,
        "suspendLoad", &T::suspendLoad
    );
};

namespace z3lx::loader {
void Config::Serialize(std::vector<uint8_t>& buffer) {
    glz::ex::write<opts>(*this, buffer);
}

Config Config::Deserialize(const std::vector<uint8_t>& buffer) {
    Config config {};
    glz::ex::read<opts>(config, buffer);
    return config;
}
} // namespace z3lx::loader
