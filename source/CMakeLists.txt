
# ---
# Compile user modules
# ---
add_library(modules)
target_link_libraries(modules PRIVATE startup std_peripherial)

set(HEADERS_INTERFACES_DIR interfaces)
set(HEADERS_DRIVERS_DIR drivers/headers)
set(HEADERS_MODULES_DIR modules/headers)
set(HEADERS_UTILS_DIR utils/headers)
set(HEADERS_TESTS_DIR debugApi/headers)
set(HEADERS_MAIN_DIR main/headers)
target_include_directories(
    modules
    PRIVATE ${HEADERS_INTERFACES_DIR}
    PRIVATE ${HEADERS_MAIN_DIR}
    PRIVATE ${HEADERS_DRIVERS_DIR}
    PRIVATE ${HEADERS_MODULES_DIR}
    PRIVATE ${HEADERS_UTILS_DIR}
    PRIVATE ${HEADERS_TESTS_DIR}
)

set(SOURCE_DRIVERS_DIR drivers/modules)
set(SOURCE_MODULES_DIR modules/modules)
set(SOURCE_UTILS_DIR utils/modules)
set(SOURCE_DEBUG_DIR debugApi/modules)
set(SOURCE_MAIN_DIR main/modules)
target_sources(
    modules
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
    PRIVATE ${SOURCE_MODULES_DIR}/lcd_ssd1306
    PRIVATE ${SOURCE_MODULES_DIR}/analogTempSensor
    PRIVATE ${SOURCE_MODULES_DIR}/oStream
    PRIVATE ${SOURCE_UTILS_DIR}/convertation
    PRIVATE ${SOURCE_UTILS_DIR}/crc
    PRIVATE ${SOURCE_UTILS_DIR}/strings
    PRIVATE ${SOURCE_DEBUG_DIR}/debugApi
    PRIVATE ${SOURCE_MAIN_DIR}/main
)

target_compile_options(
    modules
    PRIVATE -fno-exceptions                 # in libs
    PRIVATE -fno-non-call-exceptions
    PRIVATE -fno-rtti
    PRIVATE -fno-use-cxa-atexit             # Teardown code (including global destructors) can be omitted
)
