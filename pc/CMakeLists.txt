# Native (SDL2) desktop build of Empire -- for running/testing on this
# PC directly, without a PicoCalc.
#
# This is a separate CMake project from the top-level one on purpose.
# The top-level CMakeLists.txt configures an ARM bare-metal
# cross-compile toolchain the moment it calls pico_sdk_init(), and a
# single CMake configure only ever targets one toolchain. Producing a
# native .exe alongside that means a second, independent configure,
# using your normal (non-ARM) compiler:
#
#   cmake -S pc -B build-pc
#   cmake --build build-pc
#   build-pc\picocalc-empire-pc.exe
#
# It reuses the exact same patched empire/ and pdcurses/ (core) sources
# as the PicoCalc build -- same patches, same fixes -- and swaps in
# PDCurses' own sdl2 backend (modules/PDCurses/sdl2) in place of the
# picocalc/ hardware backend. That sdl2 backend is used as-is, straight
# from the PDCurses submodule -- it needs no patching of its own.

cmake_minimum_required(VERSION 3.13)
project(picocalc-empire-pc C)

set(CMAKE_C_STANDARD 11)

set(REPO_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)

find_package(SDL2 REQUIRED)

# --- Reuse the exact same copy+patch steps as the PicoCalc build ---
# (Same OUTPUT paths as the top-level CMakeLists.txt on purpose: both
# builds want the identical patched sources, so there's nothing to
# duplicate -- whichever build runs first produces them, and the other
# just reuses them.)

add_custom_command(
    OUTPUT ${REPO_ROOT}/empire/.patched
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${REPO_ROOT}/empire
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${REPO_ROOT}/modules/empire
            ${REPO_ROOT}/empire
    COMMAND bash ${REPO_ROOT}/scripts/apply_empire_patches.sh
    COMMAND ${CMAKE_COMMAND} -E touch ${REPO_ROOT}/empire/.patched
    DEPENDS ${REPO_ROOT}/patches/empire.patch
            ${REPO_ROOT}/scripts/apply_empire_patches.sh
    WORKING_DIRECTORY ${REPO_ROOT}
    COMMENT "Copying and patching empire sources"
    VERBATIM
)
add_custom_target(patch_empire_pc ALL DEPENDS ${REPO_ROOT}/empire/.patched)

file(GLOB EMPIRE_SOURCES CONFIGURE_DEPENDS ${REPO_ROOT}/empire/*.c)

# Same reasoning as the top-level CMakeLists.txt: Empire's own 1988 C
# needs -std=gnu89 (implicit declarations) and -fcommon (globals
# redeclared without 'extern' across files), scoped to just these files.
set_source_files_properties(${EMPIRE_SOURCES} PROPERTIES
    COMPILE_OPTIONS "-std=gnu89;-fcommon")

add_custom_command(
    OUTPUT ${REPO_ROOT}/pdcurses/.patched
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${REPO_ROOT}/pdcurses
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${REPO_ROOT}/modules/PDCurses/pdcurses
            ${REPO_ROOT}/pdcurses
    COMMAND bash ${REPO_ROOT}/scripts/apply_pdcurses_patches.sh
    COMMAND ${CMAKE_COMMAND} -E touch ${REPO_ROOT}/pdcurses/.patched
    DEPENDS ${REPO_ROOT}/patches/pdcurses.patch
            ${REPO_ROOT}/scripts/apply_pdcurses_patches.sh
    WORKING_DIRECTORY ${REPO_ROOT}
    COMMENT "Copying and patching pdcurses sources"
    VERBATIM
)
add_custom_target(patch_pdcurses_pc ALL DEPENDS ${REPO_ROOT}/pdcurses/.patched)

file(GLOB PDCURSES_SOURCES CONFIGURE_DEPENDS ${REPO_ROOT}/pdcurses/*.c)

# --- PDCurses' own sdl2 backend, straight from the submodule ---
# sdltest.c is PDCurses' own standalone demo program (it defines its
# own main()) -- excluded, since we supply our own main.c below.
file(GLOB SDL2_BACKEND_SOURCES ${REPO_ROOT}/modules/PDCurses/sdl2/*.c)
list(REMOVE_ITEM SDL2_BACKEND_SOURCES ${REPO_ROOT}/modules/PDCurses/sdl2/sdltest.c)

add_executable(picocalc-empire-pc
    ${EMPIRE_SOURCES}
    ${PDCURSES_SOURCES}
    ${SDL2_BACKEND_SOURCES}
    main.c
)

add_dependencies(picocalc-empire-pc patch_empire_pc patch_pdcurses_pc)

target_include_directories(picocalc-empire-pc PRIVATE
    ${REPO_ROOT}/empire
    ${REPO_ROOT}/pdcurses
    ${REPO_ROOT}/modules/PDCurses
    ${REPO_ROOT}/modules/PDCurses/sdl2
)

# SDL2main must be listed before SDL2 -- it's a static lib that itself
# calls into SDL2 (SDL_strlen, SDL_SetMainReady, etc.), and the GNU
# linker only looks forward through the libraries it's given, never
# back, to resolve a symbol. Reversed, it links "successfully" build
# after build until the exact moment it doesn't -- the reported
# undefined references (SDL_main, SDL_SetMainReady...) are the tell
# that this is a link-order problem, not a missing library.
target_link_libraries(picocalc-empire-pc PRIVATE SDL2::SDL2main SDL2::SDL2)
