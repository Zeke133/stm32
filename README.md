# stm32

Here project of light OOP framework for STM32F103C8T6.
A main cause of developement was to create easy and extandable platform for custom motorcycle board computer.

Aim is to make quick and easy use of any hardware devices like objects and build using this base functional modules.

**Advantages:**

- *Widely available.* Cheap and widely used developement board - "Blue Pill".
- *Easy to use.* Easy developement of any light automation.
- *All in one.* No need in dozens of huge linked libraries - based just on STM Std Peripherial Driver and CMSIS.
- *Modern.* Object oriented - C++ 17.
- *Crossplatform.* Easy developement on any OS you like includes only free and open source software.
- *Free toolchain.* IDE free - can use any IDE or Editors you like. I prefered Visual Studio Code.

**Internal peripheral drivers implemented:**

- SysClock
- General Purpose Input Output
- SysTick Timer
- Debug Watchdog Timer
- Direct Memory Access
- I2C
- Usart
- Aanalog Digital Converter
- FLASH
- Real Time Clock
- Timer 2

**External hardware drivers:**

- HD44780 LCD dispalays
- SSD1306 OLED dispalays
- LED indicators
- DS18B20 temperature sensor
- RPM sensors

**Example of software developement tools stack**
Console >> CMake >> Make >> GCC (arm-none-eabi toolchain) >> OpenOCD / stmDemoLoader >> DevBoard

**Code example of SSD1306 OLED display use:**

    DMA dmaForI2c1(DMA::Device::I2C1_TX);                   // dma object for I2c quck data transmition
    I2c i2cPort(1, dmaForI2c1);                             // i2c object for connection with display
    Ssd1306 oled(i2cPort, delayer);                         // display object
    oled.fill(0);                                           // filling display buffer with black pixels
    Font_7x10 fontS;                                        // object of small font
    TextRender textRender(oled, fontS);                     // object of render to draw font on display
    textRender.puts(__DATE__);
    oled.update();                                          // display buffer flush: image appears on screen

Project structure:

[link on github](https://github.com/Zeke133/stm32/blob/master/docs/shemes/SoftwareDiagram.xml "draw.io format")

![Developement board](https://github.com/Zeke133/stm32/blob/master/docs/datasheets/mcu/STM32F103C8.png "Blue pill board")
