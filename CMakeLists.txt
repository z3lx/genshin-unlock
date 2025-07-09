cmake_minimum_required(VERSION 3.28)
project(genshin-fov-unlock
    VERSION 1.7.0.0
)

set(CMAKE_CXX_STANDARD 23)

if (NOT MSVC)
    message(FATAL_ERROR "This project requires MSVC to build.")
endif()

include(FetchContent)

# TsudaKageyu/minhook
FetchContent_Declare(
    minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_COMMIT v1.3.4
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(minhook)

# stephenberry/glaze
FetchContent_Declare(
    glaze
    GIT_REPOSITORY https://github.com/stephenberry/glaze.git
    GIT_TAG v5.5.2
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glaze)

# microsoft/wil
add_compile_definitions(WIL_EXCEPTION_MODE=1)
set(WIL_BUILD_PACKAGING OFF)
set(WIL_BUILD_TESTS OFF)
FetchContent_Declare(
    wil
    GIT_REPOSITORY https://github.com/microsoft/wil.git
    GIT_TAG v1.0.250325.1
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(wil)

add_compile_options(
    /permissive-
    "$<$<CONFIG:Release>:/GL>"
    "$<$<CONFIG:Release>:/Gw>"
)
add_link_options(
    "$<$<CONFIG:Release>:/LTCG>"
)

set(VERSION_RC ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
configure_file(
    res/version.rc.in
    ${VERSION_RC}
    @ONLY
)

add_compile_definitions(WIN32_LEAN_AND_MEAN)

file(GLOB_RECURSE UTIL_SOURCES src/util/*)
add_library(util STATIC ${UTIL_SOURCES})
target_include_directories(util PRIVATE include)
target_link_libraries(util PUBLIC
    minhook
    WIL::WIL
)

file(GLOB_RECURSE LOADER_SOURCES src/loader/*)
add_executable(loader ${LOADER_SOURCES} ${VERSION_RC})
target_include_directories(loader PRIVATE include)
target_link_libraries(loader PRIVATE
    glaze::glaze
    WIL::WIL
    util
)
target_link_options(loader PRIVATE
    /MANIFESTUAC:level='requireAdministrator'
)

file(GLOB_RECURSE PLUGIN_SOURCES src/plugin/*)
add_library(plugin SHARED ${PLUGIN_SOURCES} ${VERSION_RC})
target_include_directories(plugin PRIVATE include)
target_link_libraries(plugin PRIVATE
    glaze::glaze
    WIL::WIL
    util
)
