
# ---
# Compile user modules
# ---
add_library(modules)
target_link_libraries(modules PRIVATE startup stm_ll)

set(CORE_DIR            core)
set(DEBUG_API_DIR       debugApi)
set(EXT_DISPLAYS_DIR    externalDevices/displays)
set(EXT_LEDS_DIR        externalDevices/leds)
set(EXT_PROTOCOLS_DIR   externalDevices/protocols)
set(EXT_SENSORS_DIR     externalDevices/sensors)
set(INT_DEVICES_DIR     internalDevices)
set(INTERFACES_DIR      interfaces)
set(IO_GRAPH_DIR        ioProviders/graphicRender)
set(IO_TEXT_DIR         ioProviders/textOutput)
set(UTILS_DIR           utils)
set(CODE_STYLE_DIR      examples/codeStyle)

set(H_DIR               headers)
set(SRC_DIR             modules)

target_include_directories(
    modules
    PRIVATE ${CODE_STYLE_DIR}
    PRIVATE ${INTERFACES_DIR}
    PRIVATE ${CORE_DIR}/${H_DIR}
    PRIVATE ${DEBUG_API_DIR}/${H_DIR}
    PRIVATE ${EXT_DISPLAYS_DIR}/${H_DIR}
    PRIVATE ${EXT_LEDS_DIR}/${H_DIR}
    PRIVATE ${EXT_PROTOCOLS_DIR}/${H_DIR}
    PRIVATE ${EXT_SENSORS_DIR}/${H_DIR}
    PRIVATE ${INT_DEVICES_DIR}/${H_DIR}
    PRIVATE ${IO_GRAPH_DIR}/${H_DIR}
    PRIVATE ${IO_TEXT_DIR}/${H_DIR}
    PRIVATE ${UTILS_DIR}/${H_DIR}
)

target_sources(
    modules
    PRIVATE ${CODE_STYLE_DIR}/sourceCodeTemplate
    PRIVATE ${CORE_DIR}/${SRC_DIR}/main
    # PRIVATE ${DEBUG_API_DIR}/${SRC_DIR}/_debugApi
    PRIVATE ${EXT_DISPLAYS_DIR}/${SRC_DIR}/hd44780
    PRIVATE ${EXT_DISPLAYS_DIR}/${SRC_DIR}/ssd1306
    PRIVATE ${EXT_LEDS_DIR}/${SRC_DIR}/led
    PRIVATE ${EXT_PROTOCOLS_DIR}/${SRC_DIR}/oneWire
    PRIVATE ${EXT_PROTOCOLS_DIR}/${SRC_DIR}/_oneWireSearch
    PRIVATE ${EXT_SENSORS_DIR}/${SRC_DIR}/ds18b20
    PRIVATE ${EXT_SENSORS_DIR}/${SRC_DIR}/bmp280
    PRIVATE ${EXT_SENSORS_DIR}/${SRC_DIR}/_tacho
    PRIVATE ${EXT_SENSORS_DIR}/${SRC_DIR}/_analogTempSensor
    # PRIVATE ${INT_DEVICES_DIR}/${SRC_DIR}/_storage    # no in LL lib ???
    PRIVATE ${INT_DEVICES_DIR}/${SRC_DIR}/sysClock
    PRIVATE ${INT_DEVICES_DIR}/${SRC_DIR}/gpio
    PRIVATE ${INT_DEVICES_DIR}/${SRC_DIR}/delay
    PRIVATE ${INT_DEVICES_DIR}/${SRC_DIR}/adc
    PRIVATE ${INT_DEVICES_DIR}/${SRC_DIR}/dma
    PRIVATE ${INT_DEVICES_DIR}/${SRC_DIR}/i2c
    PRIVATE ${INT_DEVICES_DIR}/${SRC_DIR}/usart
    PRIVATE ${INT_DEVICES_DIR}/${SRC_DIR}/rtc
    PRIVATE ${IO_GRAPH_DIR}/${SRC_DIR}/textRender
    PRIVATE ${IO_TEXT_DIR}/${SRC_DIR}/oStream
    PRIVATE ${UTILS_DIR}/${SRC_DIR}/convertation
    PRIVATE ${UTILS_DIR}/${SRC_DIR}/crc
    PRIVATE ${UTILS_DIR}/${SRC_DIR}/strings
)
