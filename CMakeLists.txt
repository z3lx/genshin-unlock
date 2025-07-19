cmake_minimum_required(VERSION 3.28)
project(genshin-fov-unlock
    VERSION 25.7.0.0
)

set(CMAKE_CXX_STANDARD 23)

if (NOT MSVC)
    message(FATAL_ERROR "This project requires MSVC to build.")
endif()

include(FetchContent)

set(BUILD_SHARED_LIBS OFF)
FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.11.3
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(cpr)

FetchContent_Declare(
    glaze
    GIT_REPOSITORY https://github.com/stephenberry/glaze.git
    GIT_TAG v5.5.2
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glaze)

FetchContent_Declare(
    minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_COMMIT v1.3.4
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(minhook)

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
    /utf-8
    "$<$<CONFIG:Release>:/GL>"
    "$<$<CONFIG:Release>:/Gw>"
)
add_link_options(
    "$<$<CONFIG:Release>:/LTCG>"
)

set(RESOURCES_DIR ${CMAKE_CURRENT_BINARY_DIR}/res)
file(COPY res/ DESTINATION ${RESOURCES_DIR})
configure_file(
    ${RESOURCES_DIR}/version.rc.in
    ${RESOURCES_DIR}/version.rc
    @ONLY
)

add_compile_definitions(WIN32_LEAN_AND_MEAN)

file(GLOB_RECURSE UTIL_SOURCES src/util/*)
add_library(util STATIC ${UTIL_SOURCES})
target_include_directories(util PRIVATE include)
target_link_libraries(util
    PUBLIC minhook
    PRIVATE version.lib
    PUBLIC WIL::WIL
)

file(GLOB_RECURSE LOADER_SOURCES src/loader/*)
set(LOADER_RESOURCE ${RESOURCES_DIR}/loader.rc)
add_executable(loader ${LOADER_SOURCES} ${LOADER_RESOURCE})
target_include_directories(loader PRIVATE include)
target_link_libraries(loader
    PRIVATE cpr::cpr
    PRIVATE glaze::glaze
    PRIVATE WIL::WIL
    PRIVATE util
)
target_link_options(loader PRIVATE
    /MANIFESTUAC:level='requireAdministrator'
)

file(GLOB_RECURSE PLUGIN_SOURCES src/plugin/*)
set(PLUGIN_RESOURCE ${RESOURCES_DIR}/plugin.rc)
add_library(plugin SHARED ${PLUGIN_SOURCES} ${PLUGIN_RESOURCE})
target_include_directories(plugin PRIVATE include)
target_link_libraries(plugin
    PRIVATE glaze::glaze
    PRIVATE WIL::WIL
    PRIVATE util
)
