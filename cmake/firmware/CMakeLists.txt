cmake_minimum_required (VERSION 3.9)

# Project name
project(
    MOTO_BC
    VERSION 0.1.1
    DESCRIPTION "Firmware for vintage motorcycle board computer on STM32F103C8T6 base"
    HOMEPAGE_URL "https://github.com/Zeke133/stm32"
    LANGUAGES
        ASM     # startup procedure
        C       # stm std_peripherial and cmsis libs
        CXX     # user project
)

# ROOT path
set(ROOT_DIR ${PROJECT_SOURCE_DIR}/../..)
# CMSIS path
set(CMSIS_DIR ${ROOT_DIR}/libs/cmsis)
# Std Peripherial driver path
set(STDPERIPH_DIR ${ROOT_DIR}/libs/STM32F10x_StdPeriph_Driver)

# ---
# Compile CMSIS liblary to static
# ---
add_library(cmsis STATIC)

set(HEADERS_CMSIS_DIR ${CMSIS_DIR}/CM3)
target_include_directories(
    cmsis
    PUBLIC ${HEADERS_CMSIS_DIR}/CoreSupport
    PUBLIC ${HEADERS_CMSIS_DIR}/DeviceSupport/ST/STM32F10x
)

set(SOURCE_CMSIS_DIR ${CMSIS_DIR}/CM3/DeviceSupport/ST/STM32F10x)
target_sources(
    cmsis
    PRIVATE ${SOURCE_CMSIS_DIR}/system_stm32f10x
)

target_compile_definitions(
    cmsis
    PRIVATE STM32F103C8
    PUBLIC STM32F10X_MD
)

# ---
# Compile STDPeriph library to static
# ---
add_library(std_peripherial STATIC)
target_link_libraries(std_peripherial PUBLIC cmsis)

set(HEADERS_STDPERIPH_DIR ${STDPERIPH_DIR}/inc)
target_include_directories(
    std_peripherial
    PUBLIC ${HEADERS_STDPERIPH_DIR}
)

set(SOURCE_STDPERIPH_DIR ${STDPERIPH_DIR}/src)
target_sources(
    std_peripherial
    PRIVATE ${SOURCE_STDPERIPH_DIR}/misc
    PRIVATE ${SOURCE_STDPERIPH_DIR}/stm32f10x_rcc
    PRIVATE ${SOURCE_STDPERIPH_DIR}/stm32f10x_gpio
    PRIVATE ${SOURCE_STDPERIPH_DIR}/stm32f10x_usart
    PRIVATE ${SOURCE_STDPERIPH_DIR}/stm32f10x_i2c
    PRIVATE ${SOURCE_STDPERIPH_DIR}/stm32f10x_rtc
    PRIVATE ${SOURCE_STDPERIPH_DIR}/stm32f10x_pwr
    PRIVATE ${SOURCE_STDPERIPH_DIR}/stm32f10x_dma
    PRIVATE ${SOURCE_STDPERIPH_DIR}/stm32f10x_adc
    PRIVATE ${SOURCE_STDPERIPH_DIR}/stm32f10x_tim
    PRIVATE ${SOURCE_STDPERIPH_DIR}/stm32f10x_flash
)

target_compile_definitions(
    std_peripherial
    PRIVATE USE_STDPERIPH_DRIVER
)

# ---
# Compile startup to OBJECT
# ---
set(SOURCE_STARTUP_DIR ${ROOT_DIR}/source/main/modules)
add_library(startup OBJECT)
target_sources(
    startup
    PRIVATE ${SOURCE_STARTUP_DIR}/startup_stm32f10x_md.S        # ASM startup init
    # PRIVATE ${SOURCE_STARTUP_DIR}/startup_stm32f10x_md.c        # ANSI C startup init // not checked!
)

# ---
# Compile user modules
# ---
add_executable(app.elf)
target_link_libraries(app.elf PRIVATE startup std_peripherial)

set(HEADERS_INTERFACES_DIR ${ROOT_DIR}/source/interfaces)
set(HEADERS_DRIVERS_DIR ${ROOT_DIR}/source/drivers/headers)
set(HEADERS_MODULES_DIR ${ROOT_DIR}/source/modules/headers)
set(HEADERS_UTILS_DIR ${ROOT_DIR}/source/utils/headers)
set(HEADERS_TESTS_DIR ${ROOT_DIR}/source/tests/realHardware)
set(HEADERS_MAIN_DIR ${ROOT_DIR}/source/main/headers)
target_include_directories(
    app.elf
    PRIVATE ${HEADERS_INTERFACES_DIR}
    PRIVATE ${HEADERS_MAIN_DIR}
    PRIVATE ${HEADERS_DRIVERS_DIR}
    PRIVATE ${HEADERS_MODULES_DIR}
    PRIVATE ${HEADERS_UTILS_DIR}
    PRIVATE ${HEADERS_TESTS_DIR}
)

set(SOURCE_DRIVERS_DIR ${ROOT_DIR}/source/drivers/modules)
set(SOURCE_MODULES_DIR ${ROOT_DIR}/source/modules/modules)
set(SOURCE_UTILS_DIR ${ROOT_DIR}/source/utils/modules)
set(SOURCE_TESTS_DIR ${ROOT_DIR}/source/tests/realHardware)
set(SOURCE_MAIN_DIR ${ROOT_DIR}/source/main/modules)
target_sources(
    app.elf
    PRIVATE ${SOURCE_DRIVERS_DIR}/delay
    PRIVATE ${SOURCE_DRIVERS_DIR}/sysClock
    PRIVATE ${SOURCE_DRIVERS_DIR}/gpio
    PRIVATE ${SOURCE_DRIVERS_DIR}/led
    PRIVATE ${SOURCE_DRIVERS_DIR}/usart
    PRIVATE ${SOURCE_DRIVERS_DIR}/oneWire
    PRIVATE ${SOURCE_DRIVERS_DIR}/i2c
    PRIVATE ${SOURCE_DRIVERS_DIR}/rtc
    PRIVATE ${SOURCE_DRIVERS_DIR}/adc
    PRIVATE ${SOURCE_DRIVERS_DIR}/tacho
    PRIVATE ${SOURCE_DRIVERS_DIR}/storage
    PRIVATE ${SOURCE_MODULES_DIR}/oneWireSearch
    PRIVATE ${SOURCE_MODULES_DIR}/ds18b20
    PRIVATE ${SOURCE_MODULES_DIR}/lcd
    PRIVATE ${SOURCE_MODULES_DIR}/analogTempSensor
    PRIVATE ${SOURCE_MODULES_DIR}/oStream
    PRIVATE ${SOURCE_UTILS_DIR}/convertation
    PRIVATE ${SOURCE_UTILS_DIR}/crc
    PRIVATE ${SOURCE_UTILS_DIR}/strings
    PRIVATE ${SOURCE_TESTS_DIR}/testsApi
    PRIVATE ${SOURCE_MAIN_DIR}/main
)

target_compile_options(
    app.elf
    PRIVATE -fno-exceptions                 # in libs
    PRIVATE -fno-non-call-exceptions
    PRIVATE -fno-rtti
    PRIVATE -fno-use-cxa-atexit             # Teardown code (including global destructors) can be omitted
)

set_property(
    TARGET cmsis std_peripherial app.elf
    APPEND
    PROPERTY
        COMPILE_OPTIONS
            # Common flags
            -mcpu=cortex-m3
            -mthumb
            -g                              # debug mode
            -O0                             # optimisation
            -Wall                           # show all warnings
            # Bare metal specific flags
            --specs=nano.specs              # To use newlib-nano
            -ffunction-sections             # which split functions and data into their own ELF sections.
            -fdata-sections                 # This allows the linker to eliminate
                                            # additional unused code when passed --gc-sections
            -ffreestanding                  # indicates that your program exists in an environment
                                            # where standard library facilities may be absent
                                            # and where your program may not begin at main()
)

# ---
# Link executable
# ---
set(MAP_FILE_OUTPUT_PATH ${ROOT_DIR}/bin/app.map)
set(LINKER_SCRIPT_PATH ${ROOT_DIR}/linkScripts/link.ld)
set(PLATFORM_LINK_FLAGS "-mcpu=cortex-m3 -mthumb")
set(EMBEDDED_LINK_FLAGS "--specs=nano.specs --specs=nosys.specs -nostartfiles")
set(USER_LINK_FLAG "-O0 -g -Wl,-Map=${MAP_FILE_OUTPUT_PATH} -Wl,-cref -Wl,--gc-sections")
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
# -Wl,-Map=${ROOT_DIR}/bin/app.map            # .map output
# -Wl,-cref                                   # include cross-referenses in .map
# -Wl,--gc-sections                           # bring out unused blocks of code from output
# -Wl,-T${ROOT_DIR}/linkScripts/link.ld       # linking script

set(HEX_FILE_OUT ${ROOT_DIR}/bin/app.hex)
set(BIN_FILE_OUT ${ROOT_DIR}/bin/app.bin)
set(SYM_FILE_OUT ${ROOT_DIR}/bin/app.sym)
add_custom_command(
    TARGET app.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:app.elf> ${HEX_FILE_OUT}
    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:app.elf> ${BIN_FILE_OUT}
    COMMAND ${CMAKE_NM} -n $<TARGET_FILE:app.elf> > ${SYM_FILE_OUT}
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:app.elf>
    COMMENT "Building ${HEX_FILE_OUT} \nBuilding ${BIN_FILE_OUT}"
)

# Try precompiled header for STD_PERIPH ?!!!

# move binary to bin

# make tests

# divide by folders

