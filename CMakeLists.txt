cmake_minimum_required(VERSION 3.28)
project(genshin-fov-unlock)

set(CMAKE_CXX_STANDARD 23)

if (MSVC)
    add_compile_options(
        /permissive-
        "$<$<CONFIG:Release>:/GL>"
        "$<$<CONFIG:Release>:/Gw>"
    )
    add_link_options(
        "$<$<CONFIG:Release>:/LTCG>"
    )
endif()

include(FetchContent)

# TsudaKageyu/minhook
FetchContent_Declare(
    minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_COMMIT c1a7c3843bd1a5fe3eb779b64c0d823bca3dc339
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

file(GLOB_RECURSE SOURCES src/*)
add_library(genshin_fov_unlock SHARED ${SOURCES})
target_compile_definitions(genshin_fov_unlock PRIVATE WIN32_LEAN_AND_MEAN)
target_include_directories(genshin_fov_unlock PRIVATE include)
target_link_libraries(genshin_fov_unlock
    PRIVATE minhook
    PRIVATE glaze::glaze
    PRIVATE WIL::WIL
)
