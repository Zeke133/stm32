cmake_minimum_required (VERSION 3.6)

# Project name
project (
    Firmware
    VERSION 0.1.1
    LANGUAGES C CXX
)

set(ROOT_DIR ${Firmware_SOURCE_DIR}/../../)

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


# Compile STDPeriph into separate library
add_library(stdPeriph STATIC)
target_include_directories(stdPeriph
    PUBLIC ${HEADERS_CMSIS_DIR}/CoreSupport
    PUBLIC ${HEADERS_CMSIS_DIR}/DeviceSupport/ST/STM32F10x
    PUBLIC ${HEADERS_STDPERIPH_DIR}
    )
target_sources(stdPeriph
    PRIVATE ${SOURCE_CMSIS_DIR}/system_stm32f10x
    PRIVATE ${SOURCE_STARTUP_DIR}/startup_stm32f10x_md.S        # ASM startup init
    # ${SOURCE_STARTUP_DIR}/startup_stm32f10x_md.c      # ANSI C startup init
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
target_compile_definitions(stdPeriph
    PRIVATE STM32F103C8
    PRIVATE STM32F10X_MD
    PRIVATE USE_STDPERIPH_DRIVER
    PRIVATE __ASSEMBLY__
    )
target_compile_options(stdPeriph
    # Common flags
    PRIVATE -mcpu=cortex-m3
    PRIVATE -mthumb
    PRIVATE -g                              # debug mode
    PRIVATE -O0                             # optimisation
    PRIVATE -c                              # compile only, without link
    PRIVATE -Wall                           # show all warnings
    # Bare metal specific flags
    PRIVATE --specs=nano.specs              # To use newlib-nano
    PRIVATE -ffunction-sections             # which split functions and data into their own ELF sections.
    PRIVATE -fdata-sections                 # This allows the linker to eliminate
                                            # additional unused code when passed --gc-sections
    PRIVATE -ffreestanding                  # indicates that your program exists in an environment
                                            # where standard library facilities may be absent
                                            # and where your program may not begin at main()
    )

# Executable file
add_executable(app)
target_link_libraries(app PUBLIC stdPeriph)
target_include_directories(app
    PRIVATE ${HEADERS_INTERFACES_DIR}
    PRIVATE ${HEADERS_MAIN_DIR}
    PRIVATE ${HEADERS_DRIVERS_DIR}
    PRIVATE ${HEADERS_MODULES_DIR}
    PRIVATE ${HEADERS_UTILS_DIR}
    PRIVATE ${HEADERS_TESTS_DIR}
    )
target_sources(app
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
target_compile_definitions(app
    PRIVATE STM32F103C8
    PRIVATE STM32F10X_MD
    PRIVATE USE_STDPERIPH_DRIVER
    PRIVATE __ASSEMBLY__
    PRIVATE SUPPORT_CPLUSPLUS
    )
target_compile_options(app
    # Common flags
    PRIVATE -mcpu=cortex-m3
    PRIVATE -mthumb
    PRIVATE -g                              # debug mode
    PRIVATE -O0                             # optimisation
    PRIVATE -c                              # compile only, without link
    PRIVATE -Wall                           # show all warnings
    # Bare metal specific flags
    PRIVATE --specs=nano.specs              # To use newlib-nano
    PRIVATE -ffunction-sections             # which split functions and data into their own ELF sections.
    PRIVATE -fdata-sections                 # This allows the linker to eliminate
                                            # additional unused code when passed --gc-sections
    PRIVATE -fno-exceptions                 # in libs
    PRIVATE -fno-non-call-exceptions
    PRIVATE -fno-rtti
    PRIVATE -fno-use-cxa-atexit             # Teardown code (including global destructors) can be omitted
    PRIVATE -ffreestanding                  # indicates that your program exists in an environment
                                            # where standard library facilities may be absent
                                            # and where your program may not begin at main()
    )




# set_target_properties(app
#     PROPERTIES
#     LINK_FLAGS
#     -mcpu=cortex-m3
#     -mthumb
#     -O0                                         # optimization level
#     -g                                          # debug mode
#     # ---
#     --specs=nano.specs                          # To use newlib-nano
#     --specs=nosys.specs                         # no semi-hosting
#     -nostartfiles                               # Do not use the standard system startup files
#     # ---
#     -Wl,-Map=${ROOT_DIR}/bin/app.map       # .map output
#     -Wl,-cref                                   # include cross-referenses in .map
#     -Wl,--gc-sections                           # bring out unused blocks of code from output
#     -Wl,-T${ROOT_DIR}/linkScripts/link.ld  # linking script
# )

# add_custom_target(run
#     COMMAND binary
#     DEPENDS binary
#     WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
# )

# execute_process()

