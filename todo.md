# TODO

1. bpe280 driver
    - ~~dma receive mode~~
    - ~~use dma receive in i2c driver~~
    - ~~try bit-banding using~~
    - main driver features
    - check what is going in TC interrupt i2c
2. refactoring
    - move tacho to devices
    - I2C restart in case of stuck - if device returned error. BMP280 likes to stuck. This bit can be used to reinitialize the peripheral after an error or a locked state. As an example, if the BUSY bit is set and remains locked due to a glitch on the bus, the SWRST bit can be used to exit from this state.
    - if some i2c device was disconnected - doesn't work correct with next one
    - ~~may be can use Busy FLAG check ???~~
    - const expr to create DMA channel instance
    - i2c error handling to devices
    - issue with last symbol in USART DMA transmition
    - optimize stdLib inline and so on
    - use bit-banding in perepherial drivers
    - ~~change to constexpr everything that's possible~~
    - Try precompiled header for STD_PERIPH ???
3. os core
    - tasks
    - resourceControl
4. voltmeter
5. doxygen - in process
6. timer tacho/ign
7. strobescope
8. ninja
9. esp8266 >> RIOT
