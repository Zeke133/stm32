#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <stm32f10x_flash.h>


class Settings {

friend int main(void);

public:

    struct SettingsData data;

    void readToRam(void);
    void writeToFlash(void);

private:

    struct SettingsData {
        uint32_t var1;
        uint32_t var2;
        uint32_t var3;
        uint32_t var4;
    };
    
    Settings();

    const uint32_t MY_FLASH_PAGE_ADDR = 0x800FC00;
    const uint16_t SETTINGS_WORDS = sizeof(data)/4;

};

#endif

