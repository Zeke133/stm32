
# CMSIS path
set(CMSIS_DIR cmsis)
# STM Low Level driver path
set(STM_LL_DIR STM32F1xx_LL_Driver)

# ---
# Compile CMSIS liblary to static
# ---
add_library(cmsis STATIC)

set(HEADERS_CMSIS_DIR ${CMSIS_DIR})
set(SOURCE_CMSIS_DIR ${CMSIS_DIR})

target_include_directories(
    cmsis
    PUBLIC ${HEADERS_CMSIS_DIR}/ARMCM3
    PUBLIC ${HEADERS_CMSIS_DIR}/STM32F1xx
)

target_sources(
    cmsis
    PRIVATE ${SOURCE_CMSIS_DIR}/STM32F1xx/system_stm32f1xx
)

target_compile_definitions(
    cmsis
    PUBLIC STM32F103xB    ##!< STM32F103C8, STM32F103R8, STM32F103T8, STM32F103V8, STM32F103CB, STM32F103RB, STM32F103TB and STM32F103VB
    PUBLIC USE_FULL_LL_DRIVER
)

# ---
# Compile STM Low Level library to static
# ---
add_library(stm_ll STATIC)
target_link_libraries(stm_ll PUBLIC cmsis)

set(HEADERS_STDPERIPH_DIR ${STM_LL_DIR}/inc)
set(SOURCE_STDPERIPH_DIR ${STM_LL_DIR}/src)

target_include_directories(
    stm_ll
    PUBLIC ${HEADERS_STDPERIPH_DIR}
)

target_sources(
    stm_ll
    PRIVATE ${SOURCE_STDPERIPH_DIR}/stm32f1xx_ll_rcc
    PRIVATE ${SOURCE_STDPERIPH_DIR}/stm32f1xx_ll_pwr
    PRIVATE ${SOURCE_STDPERIPH_DIR}/stm32f1xx_ll_utils
    PRIVATE ${SOURCE_STDPERIPH_DIR}/stm32f1xx_ll_gpio
    PRIVATE ${SOURCE_STDPERIPH_DIR}/stm32f1xx_ll_usart
    PRIVATE ${SOURCE_STDPERIPH_DIR}/stm32f1xx_ll_i2c
    PRIVATE ${SOURCE_STDPERIPH_DIR}/stm32f1xx_ll_rtc
    PRIVATE ${SOURCE_STDPERIPH_DIR}/stm32f1xx_ll_dma
    PRIVATE ${SOURCE_STDPERIPH_DIR}/stm32f1xx_ll_adc
    PRIVATE ${SOURCE_STDPERIPH_DIR}/stm32f1xx_ll_tim
)
