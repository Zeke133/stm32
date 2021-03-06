cmake_minimum_required (VERSION 3.9)

# Project name
project(
    MOTO_BC
    VERSION 0.1.1
    DESCRIPTION     "Firmware for vintage motorcycle board computer on STM32F103C8T6 base"
    HOMEPAGE_URL    "https://github.com/Zeke133/stm32"
    LANGUAGES
        ASM     # startup procedure
        C       # stm std_peripherial and cmsis libs
        CXX     # user project
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------
# Unit tests
enable_testing()
add_subdirectory(unitTests)
add_test(
    NAME utils
    COMMAND unitTests
)

# ------------------------------------------------------------
# Libraries CMSIS and STM_LL
add_subdirectory(libs)
# User Modules library
add_subdirectory(source)
# Startup procedure
set(SOURCE_STARTUP_DIR source/startup)
add_library(startup OBJECT)
target_sources(
    startup
    PRIVATE ${SOURCE_STARTUP_DIR}/startup_stm32f10x_md.S
)

# ------------------------------------------------------------
# Link firmware
add_executable(app.elf)
target_link_libraries(app.elf PRIVATE startup modules)

# ---
# Setup common compilation properties
# ---
# For UnitTests
set_target_properties(
        unitTests
        app.elf
    PROPERTIES
        COMPILE_FEATURES
            cxx_std_17
        COMPILE_OPTIONS
            -Wall
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)
# ---
# For user modules
set_property(
    TARGET
        modules
    PROPERTY
        COMPILE_FEATURES
            cxx_std_17
)
set_property(
    TARGET
        modules
    PROPERTY
        COMPILE_OPTIONS
            -fno-exceptions         # in libs
            -fno-non-call-exceptions
            -fno-rtti
            -fno-use-cxa-atexit     # Teardown code (including global destructors) can be omitted
)
# ---
# For libs and user modules
set_property(
    TARGET
        cmsis
        stm_ll
        modules
    APPEND
    PROPERTY
        COMPILE_OPTIONS
            # Common flags
            -mcpu=cortex-m3
            -mthumb
            -g                      # debug mode
            -Os                     # optimisation
            -Wall
            # Bare metal specific flags
            --specs=nano.specs      # To use newlib-nano
            -ffunction-sections     # which split functions and data into their own ELF sections.
            -fdata-sections         # This allows the linker to eliminate
                                    # additional unused code when passed --gc-sections
            -ffreestanding          # indicates that your program exists in an environment
                                    # where standard library facilities may be absent
                                    # and where your program may not begin at main()
            -finline-small-functions    # which inlines functions whenever the compiler thinks
                                        # the function body contains fewer instructions
                                        # than the code needed to call it.
            -findirect-inlining     # which runs additional inlining passes. For example,
                                    # if main() calls a() and a() calls b(), this compiler
                                    # could fold the body of a() into main(), then notice
                                    # that b() is also a good candidate for inlining and fold it in.
                                    # Serious improvements can be gained through such second-order effects.
)

# ---
# Link properties
set(MAP_FILE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/app.map)
set(LINKER_SCRIPT_PATH ${PROJECT_SOURCE_DIR}/linkScripts/link.ld)
set(PLATFORM_LINK_FLAGS     "-mcpu=cortex-m3 -mthumb")
set(EMBEDDED_LINK_FLAGS     "--specs=nano.specs --specs=nosys.specs -nostartfiles")
set(USER_LINK_FLAG          "-Os -g -Wl,-Map=${MAP_FILE_OUTPUT_PATH} -Wl,-cref -Wl,--gc-sections")
set_property(
    TARGET app.elf
    PROPERTY
        LINK_FLAGS
            # Stupid syntax let flags be defined only in one string
            "${PLATFORM_LINK_FLAGS} ${EMBEDDED_LINK_FLAGS} ${USER_LINK_FLAG} -Wl,-T${LINKER_SCRIPT_PATH}"
)
# -mcpu=cortex-m3
# -mthumb
# -O0                                         # optimization level
# -g                                          # debug mode
# --specs=nano.specs                          # To use newlib-nano
# --specs=nosys.specs                         # no semi-hosting
# -nostartfiles                               # Do not use the standard system startup files
# -Wl,-Map=bin/app.map            # .map output
# -Wl,-cref                                   # include cross-referenses in .map
# -Wl,--gc-sections                           # bring out unused blocks of code from output
# -Wl,-TlinkScripts/link.ld       # linking script

# ------------------------------------------------------------
# Post build tasks
set(HEX_FILE_OUT ${PROJECT_SOURCE_DIR}/bin/app.hex)
set(BIN_FILE_OUT ${PROJECT_SOURCE_DIR}/bin/app.bin)
set(SYM_FILE_OUT ${PROJECT_SOURCE_DIR}/bin/app.sym)
add_custom_command(
    TARGET app.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:app.elf> ${HEX_FILE_OUT}
    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:app.elf> ${BIN_FILE_OUT}
    COMMAND ${CMAKE_NM} -n $<TARGET_FILE:app.elf> > ${SYM_FILE_OUT}
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:app.elf>
    COMMENT "Building ${HEX_FILE_OUT} \nBuilding ${BIN_FILE_OUT}"
)
