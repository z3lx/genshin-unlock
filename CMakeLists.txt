cmake_minimum_required(VERSION 3.28)
project(genshin-fov-unlock)

set(CMAKE_CXX_STANDARD 23)

if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

include(FetchContent)

# TsudaKageyu/minhook
FetchContent_Declare(
    minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_COMMIT c1a7c3843bd1a5fe3eb779b64c0d823bca3dc339
)
FetchContent_MakeAvailable(minhook)

# nlohmann/json
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)
set(JSON_BuildTests OFF CACHE INTERNAL "")

# microsoft/wil
add_compile_definitions(WIL_EXCEPTION_MODE=1)
set(WIL_BUILD_PACKAGING OFF)
set(WIL_BUILD_TESTS OFF)
FetchContent_Declare(
    wil
    GIT_REPOSITORY https://github.com/microsoft/wil.git
    GIT_TAG v1.0.250325.1
)
FetchContent_MakeAvailable(wil)

file(GLOB_RECURSE UTILS_SOURCES include/utils/* src/utils/*)
add_library(utils STATIC ${UTILS_SOURCES})
target_compile_definitions(utils PRIVATE WIN32_LEAN_AND_MEAN)
target_include_directories(utils PRIVATE include)
target_link_libraries(utils PUBLIC minhook WIL::WIL)

file(GLOB_RECURSE PLUGIN_SOURCES include/plugin/* src/plugin/*)
add_library(genshin_fov_unlock SHARED ${PLUGIN_SOURCES})
target_compile_definitions(genshin_fov_unlock PRIVATE WIN32_LEAN_AND_MEAN)
target_include_directories(genshin_fov_unlock PRIVATE include)
target_link_libraries(genshin_fov_unlock PRIVATE
    nlohmann_json::nlohmann_json
    utils
)
