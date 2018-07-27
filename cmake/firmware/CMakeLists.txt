cmake_minimum_required (VERSION 3.6)

# Project name
project(
    MOTO_BC
    VERSION 0.1.1
    DESCRIPTION "Firmware for vintage motorcycle board computer on STM32F103C8T6 base"
    HOMEPAGE_URL "https://github.com/Zeke133/stm32"
    LANGUAGES C CXX
)

# ROOT path
set(ROOT_DIR ${PROJECT_SOURCE_DIR}/../..)
# CMSIS path
set(CMSIS_DIR ${ROOT_DIR}/libs/cmsis)
# Std Peripherial driver path
set(STDPERIPH_DIR ${ROOT_DIR}/libs/STM32F10x_StdPeriph_Driver)

# ---
# Headers paths
# ---
# libs
set(HEADERS_CMSIS_DIR ${CMSIS_DIR}/CM3)
set(HEADERS_STDPERIPH_DIR ${STDPERIPH_DIR}/inc)
# user files
set(HEADERS_INTERFACES_DIR ${ROOT_DIR}/source/interfaces)
set(HEADERS_DRIVERS_DIR ${ROOT_DIR}/source/drivers/headers)
set(HEADERS_MODULES_DIR ${ROOT_DIR}/source/modules/headers)
set(HEADERS_UTILS_DIR ${ROOT_DIR}/source/utils/headers)
set(HEADERS_TESTS_DIR ${ROOT_DIR}/source/tests/realHardware)
set(HEADERS_MAIN_DIR ${ROOT_DIR}/source/main/headers)

# ---
# Sources paths
# ---
# CMSIS path
set(SOURCE_CMSIS_DIR ${CMSIS_DIR}/CM3/DeviceSupport/ST/STM32F10x)
# startup files path
set(SOURCE_STARTUP_DIR ${ROOT_DIR}/source/main/modules)
# std peripherial drivers path
set(SOURCE_STDPERIPH_DIR ${STDPERIPH_DIR}/src)
# user sources path
set(SOURCE_DRIVERS_DIR ${ROOT_DIR}/source/drivers/modules)
set(SOURCE_MODULES_DIR ${ROOT_DIR}/source/modules/modules)
set(SOURCE_UTILS_DIR ${ROOT_DIR}/source/utils/modules)
set(SOURCE_TESTS_DIR ${ROOT_DIR}/source/tests/realHardware)
set(SOURCE_MAIN_DIR ${ROOT_DIR}/source/main/modules)


# ---
# Compile CMSIS liblary to static
# ---
add_library(cmsis STATIC)
target_include_directories(
    cmsis
    PUBLIC ${HEADERS_CMSIS_DIR}/CoreSupport
    PUBLIC ${HEADERS_CMSIS_DIR}/DeviceSupport/ST/STM32F10x
    )
target_sources(
    cmsis
    PRIVATE ${SOURCE_CMSIS_DIR}/system_stm32f10x
    PRIVATE ${SOURCE_STARTUP_DIR}/startup_stm32f10x_md.S        # ASM startup init
    # ${SOURCE_STARTUP_DIR}/startup_stm32f10x_md.c              # ANSI C startup init
)
get_target_property(CMSIS_SOURCES cmsis SOURCES)
set_source_files_properties(${CMSIS_SOURCES} PROPERTIES LANGUAGE CXX)

# ---
# Compile STDPeriph library to static
# ---
add_library(std_peripherial STATIC)
target_link_libraries(std_peripherial PUBLIC cmsis)
target_include_directories(
    std_peripherial
    PUBLIC ${HEADERS_STDPERIPH_DIR}
)
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
# Force CMake to interpret SOURCE files like C++ files to compile with g++
get_target_property(STD_SOURCES std_peripherial SOURCES)
set_source_files_properties(${STD_SOURCES} PROPERTIES LANGUAGE CXX)

# ---
# Compile executable
# ---
add_executable(app)
target_link_libraries(app PRIVATE cmsis std_peripherial)
target_include_directories(
    app
    PRIVATE ${HEADERS_INTERFACES_DIR}
    PRIVATE ${HEADERS_MAIN_DIR}
    PRIVATE ${HEADERS_DRIVERS_DIR}
    PRIVATE ${HEADERS_MODULES_DIR}
    PRIVATE ${HEADERS_UTILS_DIR}
    PRIVATE ${HEADERS_TESTS_DIR}
)
target_sources(
    app
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


set_property(
    TARGET cmsis std_peripherial app
    PROPERTY
        COMPILE_DEFINITIONS
            STM32F103C8
            STM32F10X_MD
            USE_STDPERIPH_DRIVER
            __ASSEMBLY__
            SUPPORT_CPLUSPLUS
)
set_property(
    TARGET cmsis std_peripherial app
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
            -fno-exceptions                 # in libs
            -fno-non-call-exceptions
            -fno-rtti
            -fno-use-cxa-atexit             # Teardown code (including global destructors) can be omitted
            -ffreestanding                  # indicates that your program exists in an environment
                                            # where standard library facilities may be absent
                                            # and where your program may not begin at main()
)
# set(BARE_METAL_LINK_FLAGS
#     --specs=nano.specs --specs=nosys.specs -nostartfiles)
# set(ARM_LINK_FLAGS
#     -mcpu=cortex-m3 -mthumb)
# set(SCRIPT_LINK_FLAG
#     -Wl,-T${ROOT_DIR}/linkScripts/link.ld)
# set(MAP_OUT_LINK_FLAG
#     -Wl,-Map=${ROOT_DIR}/bin/app.map)
# set(USER_LINK_FLAGS
#     -O0 -g -Wl,-cref -Wl,--gc-sections)
# set_property(
#     TARGET app
#     PROPERTY
#         LINK_FLAGS
#             "${BARE_METAL_LINK_FLAGS} ${ARM_LINK_FLAGS} ${SCRIPT_LINK_FLAG} ${MAP_OUT_LINK_FLAG} ${USER_LINK_FLAGS}"
# )

# -fdata-sections -g -fno-common -fmessage-length=0")

set(CMAKE_EXE_LINKER_FLAGS
    "-mcpu=cortex-m3 -mthumb -O0 -g --specs=nano.specs --specs=nosys.specs -nostartfiles -Wl,-Map=${ROOT_DIR}/bin/app.map -Wl,-cref -Wl,--gc-sections -Wl,-T${ROOT_DIR}/linkScripts/link.ld"
    # "-mcpu=cortex-m3
    # -mthumb
    # -O0
    # -g
    # --specs=nano.specs
    # --specs=nosys.specs
    # -nostartfiles
    # -Wl,-Map=${ROOT_DIR}/bin/app.map
    # -Wl,-cref
    # -Wl,--gc-sections
    # -Wl,-T${ROOT_DIR}/linkScripts/link.ld"
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


# add_custom_target(run
#     COMMAND binary
#     DEPENDS binary
#     WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
# )

# execute_process()

