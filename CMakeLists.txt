# =============================================================================
# Project configuration
# =============================================================================

cmake_minimum_required(VERSION 3.30)

if (NOT (CMAKE_GENERATOR STREQUAL "Ninja") AND
    NOT (CMAKE_GENERATOR STREQUAL "Ninja Multi-Config"))
    message(FATAL_ERROR "This project requires the Ninja generator.")
endif()

# https://github.com/Kitware/CMake/blob/v3.30.0/Help/dev/experimental.rst
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")

project(genshin-fov-unlock
    VERSION 6.0.0.0
    LANGUAGES CXX
)

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR "This project requires the MSVC compiler.")
endif()

add_compile_options(
    /permissive-
    /utf-8
    "$<$<CONFIG:Release>:/GL>"
    "$<$<CONFIG:Release>:/Gw>"
)
add_link_options(
    "$<$<CONFIG:Release>:/LTCG>"
)

# ==============================================================================
# Dependencies
# ==============================================================================

include(FetchContent)

FetchContent_Declare(
    glaze
    GIT_REPOSITORY https://github.com/stephenberry/glaze.git
    GIT_TAG v5.5.2
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glaze)

FetchContent_Declare(
    mminhook
    GIT_REPOSITORY https://github.com/z3lx/mminhook.git
    GIT_TAG 385770a435c1be054e916b93b44d0318176d05ed
)
FetchContent_MakeAvailable(mminhook)

FetchContent_Declare(
    pwu
    GIT_REPOSITORY https://github.com/z3lx/pwu.git
    GIT_TAG 57b6d48e4ff20981594b5311207a235844b814ef
)
FetchContent_MakeAvailable(pwu)

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

# ==============================================================================
# Resources configuration
# ==============================================================================

set(RESOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/res)
file(COPY res/ DESTINATION ${RESOURCE_DIR})
configure_file(
    ${RESOURCE_DIR}/version.rc.in
    ${RESOURCE_DIR}/version.rc
    @ONLY
)

# ==============================================================================
# Target properties
# ==============================================================================

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)
set(CMAKE_CXX_SCAN_FOR_MODULES TRUE)
set(CMAKE_CXX_MODULE_STD TRUE)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)

# ==============================================================================
# util
# ==============================================================================

set(UTIL_MODULE_FILES
    src/util.ixx
    src/util/ExponentialFilter.ixx
    src/util/Version.ixx
)

set(UTIL_SOURCE_FILES
    src/util/Version.cpp
)

add_library(util STATIC)
target_sources(util
    PUBLIC
        FILE_SET CXX_MODULES
        FILES ${UTIL_MODULE_FILES}
    PRIVATE
        ${UTIL_SOURCE_FILES}
)

# ==============================================================================
# common
# ==============================================================================

set(COMMON_MODULE_FILES
    src/common.ixx
    src/common/Constants.ixx
)

add_library(common STATIC)
target_sources(common
    PUBLIC
    FILE_SET CXX_MODULES
    FILES ${COMMON_MODULE_FILES}
)

# ==============================================================================
# loader
# ==============================================================================

set(LOADER_MODULE_FILES
    src/loader.ixx
    src/loader/Config.ixx
)

set(LOADER_SOURCE_FILES
    src/loader/Config.cpp
    src/loader/Main.cpp
)

set(LOADER_RESOURCE_FILES
    ${RESOURCE_DIR}/loader.rc
)

add_executable(loader)

target_sources(loader
    PRIVATE
        FILE_SET HEADERS
        BASE_DIRS include
        FILE_SET CXX_MODULES
        FILES ${LOADER_MODULE_FILES}
    PRIVATE
        ${LOADER_SOURCE_FILES}
        ${LOADER_RESOURCE_FILES}
)

target_link_libraries(loader PRIVATE
    common
    glaze::glaze
    pwu::pwu
    WIL::WIL
    util
)

target_link_options(loader PRIVATE
    /MANIFESTUAC:level='requireAdministrator'
)

# ==============================================================================
# plugin
# ==============================================================================

set(PLUGIN_MODULE_FILES
    src/plugin.ixx
    src/plugin/Config.ixx
    src/plugin/Helper.ixx
    src/plugin/Plugin.ixx
    src/plugin/components/ComponentBase.ixx
    src/plugin/components/CursorState.ixx
    src/plugin/components/FovUnlocker.ixx
    src/plugin/components/FpsUnlocker.ixx
    src/plugin/components/PersistentObject.ixx
    src/plugin/components/VirtualKeyState.ixx
    src/plugin/components/WindowState.ixx
)

set(PLUGIN_SOURCE_FILES
    src/plugin/Config.cpp
    src/plugin/DllMain.cpp
    src/plugin/Helper.cpp
    src/plugin/Plugin.cpp
    src/plugin/components/CursorState.cpp
    src/plugin/components/FovUnlocker.cpp
    src/plugin/components/FpsUnlocker.cpp
    src/plugin/components/VirtualKeyState.cpp
    src/plugin/components/WindowState.cpp
)

set(PLUGIN_RESOURCE_FILES
    ${RESOURCE_DIR}/plugin.rc
)

add_library(plugin SHARED)
target_sources(plugin
    PRIVATE
        FILE_SET CXX_MODULES
        FILES ${PLUGIN_MODULE_FILES}
    PRIVATE
        ${PLUGIN_SOURCE_FILES}
        ${PLUGIN_RESOURCE_FILES}
)

target_link_libraries(plugin PRIVATE
    common
    glaze::glaze
    mmh::mmh
    pwu::pwu
    WIL::WIL
    util
)
