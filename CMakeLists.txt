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
# old-style function definitions). Modern GCC treats those as hard errors
# by default. -std=gnu89 turns them back into warnings, but we only want
# that relaxed for Empire's own files -- not for PDCurses, the Pico SDK,
# or our own main.c, which should stay held to the normal C11 standard.
set_source_files_properties(${EMPIRE_SOURCES} PROPERTIES COMPILE_OPTIONS "-std=gnu89")

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
