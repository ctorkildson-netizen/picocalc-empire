# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.3.0)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(picocalc-empire C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

set(STARTER_KIT_SOURCES
    modules/picocalc-text-starter/drivers/audio.c
    modules/picocalc-text-starter/drivers/clib.c
    modules/picocalc-text-starter/drivers/fat32.c
    modules/picocalc-text-starter/drivers/font-5x10.c
    modules/picocalc-text-starter/drivers/font-8x10.c
    modules/picocalc-text-starter/drivers/lcd.c
    modules/picocalc-text-starter/drivers/sdcard.c
    modules/picocalc-text-starter/drivers/southbridge.c
)

file(GLOB PICOCALC_SOURCES CONFIGURE_DEPENDS
    picocalc/*.c
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_LIST_DIR}/empire/.patched
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_CURRENT_LIST_DIR}/empire
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_LIST_DIR}/modules/empire
            ${CMAKE_CURRENT_LIST_DIR}/empire
    COMMAND bash ${CMAKE_CURRENT_LIST_DIR}/scripts/apply_empire_patches.sh
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_LIST_DIR}/empire/.patched
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/patches/empire.patch
            ${CMAKE_CURRENT_LIST_DIR}/scripts/apply_empire_patches.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    COMMENT "Copying and patching empire sources"
    VERBATIM
)

add_custom_target(patch_empire ALL
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/empire/.patched
)

file(GLOB EMPIRE_SOURCES CONFIGURE_DEPENDS
    empire/*.c
)

# Empire's own source is 1988-vintage K&R C (implicit function declarations,
# old-style function definitions, and extern.h's globals declared without
# 'extern' -- relying on the old "common symbol" linker behavior where the
# same uninitialized global in multiple .c files was just merged, not an
# error). Modern GCC defaults to -Werror on the first and -fno-common
# (which forbids the second) since GCC 10/14. -std=gnu89 and -fcommon turn
# both back into what 1988 code expects, scoped to Empire's own files only
# -- not PDCurses, the Pico SDK, or our own main.c, which should stay held
# to the normal C11 standard.
set_source_files_properties(${EMPIRE_SOURCES} PROPERTIES COMPILE_OPTIONS "-std=gnu89;-fcommon")

# Ninja tracks whether to recompile a .c file by looking at its own mtime
# and the mtimes of the headers it #includes -- but the "delete and
# recreate empire/ from modules/empire, then patch it" custom command
# above happens at BUILD time, which Ninja's dependency graph doesn't
# automatically know depends on. Without this, editing patches/empire.patch
# and rebuilding can silently link a STALE binary: the patch step reruns,
# but nothing recompiles, because the object files it already built still
# look up to date. Tying every empire/*.c object file to the .patched
# stamp file fixes that -- a fresh timestamp on .patched now forces a
# rebuild of everything derived from it.
set_source_files_properties(${EMPIRE_SOURCES} PROPERTIES OBJECT_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/empire/.patched)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_LIST_DIR}/pdcurses/.patched
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_CURRENT_LIST_DIR}/pdcurses
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_LIST_DIR}/modules/PDCurses/pdcurses
            ${CMAKE_CURRENT_LIST_DIR}/pdcurses
    COMMAND bash ${CMAKE_CURRENT_LIST_DIR}/scripts/apply_pdcurses_patches.sh
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_LIST_DIR}/pdcurses/.patched
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/patches/pdcurses.patch
            ${CMAKE_CURRENT_LIST_DIR}/scripts/apply_pdcurses_patches.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    COMMENT "Copying and patching pdcurses sources"
    VERBATIM
)

add_custom_target(patch_pdcurses ALL
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/pdcurses/.patched
)

file(GLOB PDCURSES_SOURCES CONFIGURE_DEPENDS
    pdcurses/*.c
)

# Same staleness fix as EMPIRE_SOURCES above, for the pdcurses/ copy+patch step.
set_source_files_properties(${PDCURSES_SOURCES} PROPERTIES OBJECT_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/pdcurses/.patched)

add_executable(picocalc-empire
    ${PDCURSES_SOURCES}
    ${STARTER_KIT_SOURCES}
    ${PICOCALC_SOURCES}
    ${EMPIRE_SOURCES}
    main.c
    )

add_dependencies(picocalc-empire patch_empire patch_pdcurses)

# Add executable. Default name is the project name, version, url
pico_set_program_name(picocalc-empire "picocalc-empire")
pico_set_program_url(picocalc-empire "https://github.com/ctorkildson-netizen/picocalc-empire")

# Generate PIO header
pico_generate_pio_header(picocalc-empire ${CMAKE_CURRENT_LIST_DIR}/modules/picocalc-text-starter/drivers/audio.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(picocalc-empire 0)
pico_enable_stdio_usb(picocalc-empire 0)

# Add the standard library to the build
target_link_libraries(picocalc-empire
    pico_stdlib
    pico_printf
    pico_rand
    pico_bootrom
    hardware_gpio
    hardware_i2c
    hardware_spi
    hardware_pio
    hardware_clocks)

# Add the standard include files to the build
target_include_directories(picocalc-empire PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/empire
    ${CMAKE_CURRENT_LIST_DIR}/pdcurses
    ${CMAKE_CURRENT_LIST_DIR}/modules/PDCurses
    ${CMAKE_CURRENT_LIST_DIR}/modules/picocalc-text-starter
    ${CMAKE_CURRENT_LIST_DIR}/picocalc
)

pico_add_extra_outputs(picocalc-empire)
