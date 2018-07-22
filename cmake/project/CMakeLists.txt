cmake_minimum_required (VERSION 2.6)

# Project name
project (bluePill)

# CMSIS path
set(CMSIS_DIR ${SOURCE_DIR}/libs/cmsis)
# Std Peripherial driver path
set(STDPERIPH_DIR ${SOURCE_DIR}/libs/STM32F10x_StdPeriph_Driver)

# ---
# Headers paths
# ---
# libs
set(HEADERS_CMSIS_DIR ${CMSIS_DIR}/CM3)
set(HEADERS_STDPERIPH_DIR ${STDPERIPH_DIR}/inc)
# user files
set(HEADERS_INTERFACES_DIR ${SOURCE_DIR}/source/interfaces)
set(HEADERS_DRIVERS_DIR ${SOURCE_DIR}/source/drivers/headers)
set(HEADERS_MODULES_DIR ${SOURCE_DIR}/source/modules/headers)
set(HEADERS_UTILS_DIR ${SOURCE_DIR}/source/utils/headers)
set(HEADERS_TESTS_DIR ${SOURCE_DIR}/source/tests/realHardware)
set(HEADERS_MAIN_DIR ${SOURCE_DIR}/source/main/headers)

# ---
# Sources paths
# ---
# CMSIS path
set(SOURCE_CMSIS_DIR ${CMSIS_DIR}/CM3/DeviceSupport/ST/STM32F10x)
# startup files path
set(SOURCE_STARTUP_DIR ${SOURCE_DIR}/source/main/modules)
# std peripherial drivers path
set(SOURCE_STDPERIPH_DIR ${STDPERIPH_DIR}/src)
# user sources path
set(SOURCE_DRIVERS_DIR ${SOURCE_DIR}/source/drivers/modules)
set(SOURCE_MODULES_DIR ${SOURCE_DIR}/source/modules/modules)
set(SOURCE_UTILS_DIR ${SOURCE_DIR}/source/utils/modules)
set(SOURCE_TESTS_DIR ${SOURCE_DIR}/source/tests/realHardware)]
set(SOURCE_MAIN_DIR ${SOURCE_DIR}/source/main/modules)

# ---
# Sources lists
# ---
# CMSIS files
set(SOURCES_TO_BUILD
    ${SOURCES_TO_BUILD}
    ${SOURCE_STARTUP_DIR}/system_stm32f10x.c
)
# List of startup files
set(SOURCES_TO_BUILD
    ${SOURCES_TO_BUILD}
    ${SOURCE_STARTUP_DIR}/startup_stm32f10x_md.S        # ASM startup init
    # ${SOURCE_STARTUP_DIR}/startup_stm32f10x_md.c      # ANSI C startup init
)
# List of StdPeripherial Lib files to build
set(SOURCES_TO_BUILD
    ${SOURCES_TO_BUILD}
    ${SOURCE_STDPERIPH_DIR}/misc
    ${SOURCE_STDPERIPH_DIR}/stm32f10x_rcc
    ${SOURCE_STDPERIPH_DIR}/stm32f10x_gpio
    ${SOURCE_STDPERIPH_DIR}/stm32f10x_usart
    ${SOURCE_STDPERIPH_DIR}/stm32f10x_i2c
    ${SOURCE_STDPERIPH_DIR}/stm32f10x_rtc
    ${SOURCE_STDPERIPH_DIR}/stm32f10x_pwr
    ${SOURCE_STDPERIPH_DIR}/stm32f10x_dma
    ${SOURCE_STDPERIPH_DIR}/stm32f10x_adc
    ${SOURCE_STDPERIPH_DIR}/stm32f10x_tim
    ${SOURCE_STDPERIPH_DIR}/stm32f10x_flash
)
# List of user source files to build
set(SOURCES_TO_BUILD
    ${SOURCES_TO_BUILD}
    ${SOURCE_DRIVERS_DIR}/delay
    ${SOURCE_DRIVERS_DIR}/sysClock
    ${SOURCE_DRIVERS_DIR}/gpio
    ${SOURCE_DRIVERS_DIR}/led
    ${SOURCE_DRIVERS_DIR}/usart
    ${SOURCE_DRIVERS_DIR}/oneWire
    ${SOURCE_DRIVERS_DIR}/i2c
    ${SOURCE_DRIVERS_DIR}/rtc
    ${SOURCE_DRIVERS_DIR}/adc
    ${SOURCE_DRIVERS_DIR}/tacho
    ${SOURCE_DRIVERS_DIR}/storage
    ${SOURCE_MODULES_DIR}/oneWireSearch
    ${SOURCE_MODULES_DIR}/ds18b20
    ${SOURCE_MODULES_DIR}/lcd
    ${SOURCE_MODULES_DIR}/analogTempSensor
    ${SOURCE_MODULES_DIR}/oStream     
    ${SOURCE_UTILS_DIR}/convertation
    ${SOURCE_UTILS_DIR}/crc
    ${SOURCE_UTILS_DIR}/strings           
    ${SOURCE_TESTS_DIR}/testsApi
    ${SOURCE_MAIN_DIR}/main
)

# ---
# Compillation flags
# ---
# Common flags
set(CMAKE_CXX_FLAGS
    ${CMAKE_CXX_FLAGS}
    -mcpu=cortex-m3
    -mthumb
    -g                  # debug mode
    -O0                 # optimisation
    -c                  # compile only, without link
    -Wall               # show all warnings
)
# Embadded specific flags
set(CMAKE_CXX_FLAGS
    ${CMAKE_CXX_FLAGS}
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
# Defines
set(CMAKE_CXX_FLAGS
    ${CMAKE_CXX_FLAGS}
    -DSTM32F103C8
    -DSTM32F10X_MD
    -DUSE_STDPERIPH_DRIVER
    -D__ASSEMBLY__
    -DSUPPORT_CPLUSPLUS
)

# ---
# Includes
# ---
# Libs includes
include_directories (${HEADERS_CMSIS_DIR}/CoreSupport)
include_directories (${HEADERS_CMSIS_DIR}/DeviceSupport/ST/STM32F10x)
include_directories (${HEADERS_STDPERIPH_DIR})
# User includes
include_directories (${HEADERS_INTERFACES_DIR})
include_directories (${HEADERS_MAIN_DIR})
include_directories (${HEADERS_DRIVERS_DIR})
include_directories (${HEADERS_MODULES_DIR})
include_directories (${HEADERS_UTILS_DIR})
include_directories (${HEADERS_TESTS_DIR})

# ---
# Linker Flags
#
set(LINKER_FLAGS)
    -mcpu=cortex-m3
    -mthumb
    -O0                                         # optimization level
    -g                                          # debug mode
    # ---
    --specs=nano.specs                          # To use newlib-nano
    --specs=nosys.specs                         # no semi-hosting
    -nostartfiles                               # Do not use the standard system startup files
    # ---
    -Wl,-Map=${workspaceRoot}/bin/app.map       # .map output
    -Wl,-cref                                   # include cross-referenses in .map
    -Wl,--gc-sections                           # bring out unused blocks of code from output
    -Wl,-T${workspaceRoot}/linkScripts/link.ld  # linking script
)

# How to specify a compiller arm-none-eabi-g++ ???

# How to execute binary???

# How make compilation and linking separate???

# Executable file
add_executable(app ${SOURCES_TO_BUILD})

