
# CMSIS path
set(CMSIS_DIR cmsis)
# Std Peripherial driver path
set(STDPERIPH_DIR STM32F10x_StdPeriph_Driver)

# ---
# Compile CMSIS liblary to static
# ---
add_library(cmsis STATIC)

set(HEADERS_CMSIS_DIR ${CMSIS_DIR})
target_include_directories(
    cmsis
    PUBLIC ${HEADERS_CMSIS_DIR}
)

set(SOURCE_CMSIS_DIR ${CMSIS_DIR})
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
