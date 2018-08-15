#include <lcd_ssd1306.h>

Lcd_ssd1306::Lcd_ssd1306(I2c& i2c, IDelayer& delay)
    :   i2c(i2c),
        wait(delay) {

    init();
}

void Lcd_ssd1306::writeCommand(unsigned char byte) {

    i2c.startTransmit();
    i2c.write(byte);
    i2c.stopTransmit();
}

void Lcd_ssd1306::init(void) {

    // Wait for the screen to boot
    wait.ms(100);

    // Init LCD
    writeCommand(DisplayCmd::SetDispOff);

    writeCommand(DisplayCmd::SetMemAdrMode);
    writeCommand(DisplayCmd::SetMemAdrModeHoriz);

    writeCommand(DisplayCmd::SetPageStartAddr | 0);

    writeCommand(DisplayCmd::SetCOMOutScanDirRemap);

    writeCommand(DisplayCmd::SetLowColStartAddr | 0);
    writeCommand(DisplayCmd::SetHighColStartAddr | 0);

    writeCommand(DisplayCmd::SetDispStartLine | 0);

    writeCommand(DisplayCmd::SetContrastCtrl);
    writeCommand(0xFF);

    writeCommand(DisplayCmd::SetSegmentRemapOn);

    writeCommand(DisplayCmd::SetNormalDisp);

    writeCommand(DisplayCmd::SetMultiplexRatio);
    writeCommand(0x3F);

    writeCommand(DisplayCmd::EntireDispResume);

    writeCommand(DisplayCmd::SetDispOffset);
    writeCommand(0x00);

    writeCommand(DisplayCmd::SetDispClckDiv);
    writeCommand(0xF0);

    writeCommand(DisplayCmd::SetPrechargePer);
    writeCommand(0x22);

    writeCommand(DisplayCmd::SetCOMPinsHWConf);
    writeCommand(DisplayCmd::SetCOMAlt | DisplayCmd::SetCOMNoRemap);

    writeCommand(DisplayCmd::SetVcomhDeselectLev);
    writeCommand(DisplayCmd::SetVcomh077Vcc);

    writeCommand(DisplayCmd::ChargePumpSet);
    writeCommand(DisplayCmd::EnableChargePump);

    writeCommand(DisplayCmd::SetDispOn);

    // Clear screen
    fill(Black);

    // Flush buffer to screen
    update();
}

// Fill the whole screen with the given color
void Lcd_ssd1306::fill(SSD1306_COLOR color) 
{
    uint32_t i;

    for(i = 0; i < sizeof(SSD1306_Buffer); i++)
    {
        SSD1306_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
    }
}

// Write the screenbuffer with changed to the screen
void Lcd_ssd1306::update(void) 
{
    uint8_t i;

    for (i = 0; i < 8; i++) {
        writeCommand(0xB0 + i);
        writeCommand(0x00);
        writeCommand(0x10);

        HAL_I2C_Mem_Write(&hi2c1,SSD1306_I2C_ADDR,0x40,1,&SSD1306_Buffer[SSD1306_WIDTH * i],SSD1306_WIDTH,100);
    }
}

// Draw one pixel in the screenbuffer
// X => X Coordinate
// Y => Y Coordinate
// color => Pixel color
void Lcd_ssd1306::drawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color)
{
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) 
    {
        // Don't write outside the buffer
        return;
    }

    // Check if pixel should be inverted
    if (SSD1306.Inverted) 
    {
        color = (SSD1306_COLOR)!color;
    }

    // Draw in the right color
    if (color == White)
    {
        SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
    } 
    else 
    {
        SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
    }
}

// Draw 1 char to the screen buffer
// ch 		=> char om weg te schrijven
// Font 	=> Font waarmee we gaan schrijven
// color 	=> Black or White
char Lcd_ssd1306::putc(char ch, FontDef Font, SSD1306_COLOR color)
{
    uint32_t i, b, j;

    // Check remaining space on current line
    if (SSD1306_WIDTH <= (SSD1306.CurrentX + Font.FontWidth) ||
        SSD1306_HEIGHT <= (SSD1306.CurrentY + Font.FontHeight))
    {
        // Not enough space on current line
        return 0;
    }

    // Use the font to write
    for (i = 0; i < Font.FontHeight; i++)
    {
        b = Font.data[(ch - 32) * Font.FontHeight + i];
        for (j = 0; j < Font.FontWidth; j++)
        {
            if ((b << j) & 0x8000) 
            {
                ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR) color);
            } 
            else 
            {
                ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR)!color);
            }
        }
    }

    // The current space is now taken
    SSD1306.CurrentX += Font.FontWidth;

    // Return written char for validation
    return ch;
}

// Write full string to screenbuffer
char Lcd_ssd1306::puts(char* str, FontDef Font, SSD1306_COLOR color)
{
    // Write until null-byte
    while (*str) 
    {
        if (ssd1306_WriteChar(*str, Font, color) != *str)
        {
            // Char could not be written
            return *str;
        }
        
        // Next char
        str++;
    }

    // Everything ok
    return *str;
}

// Position the cursor
void Lcd_ssd1306::cursorGoTo(uint8_t x, uint8_t y) 
{
    SSD1306.CurrentX = x;
    SSD1306.CurrentY = y;
}


