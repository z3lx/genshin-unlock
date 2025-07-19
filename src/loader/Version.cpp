#include "loader/Version.hpp"
#include "util/Version.hpp"
#include "util/win/Version.hpp"
#include "util/win/Loader.hpp"

#include <cpr/cpr.h>
#include <glaze/glaze.hpp>
#include <glaze/glaze_exceptions.hpp>

#include <stdexcept>
#include <string>

namespace {
constexpr glz::opts opts {
    .null_terminated = false,
    .comments = false,
    .error_on_unknown_keys = false,
    .skip_null_members = true,
    .prettify = false,
    .minified = false,
    .indentation_char = ' ',
    .indentation_width = 4,
    .new_lines_in_arrays = true,
    .append_arrays = true,
    .error_on_missing_keys = true,
    .error_on_const_read = true,
    .bools_as_numbers = false,
    .quoted_num = false,
    .number = false,
    .raw = false,
    .raw_string = false,
    .structs_as_arrays = false,
    .partial_read = true
};

struct LatestRelease {
    std::string tagName;
};
} // namespace

template <>
struct glz::meta<LatestRelease> {
    using T = LatestRelease;
    static constexpr auto value = object(
        "tag_name", &T::tagName
    );
};

namespace z3lx::loader {
util::Version GetLocalVersion() {
    return util::GetFileVersion(util::GetCurrentModuleFilePath());
}

util::Version GetRemoteVersion() {
    constexpr auto endpoint =
        "https://api.github.com/repos/z3lx/genshin-fov-unlock/releases/latest";
    const cpr::Response response = cpr::Get(
        cpr::Url { endpoint },
        cpr::Header { { "X-GitHub-Api-Version", "2022-11-28" } }
    );
    if (response.status_code != 200) {
        throw std::runtime_error {
            "Failed to fetch remote version with status code " +
            std::to_string(response.status_code)
        };
    }
    LatestRelease latest {};
    glz::ex::read<opts>(latest, response.text);
    return util::Version { latest.tagName };
}
} // namespace z3lx::loader
