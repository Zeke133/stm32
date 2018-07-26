#ifndef _SETTINGS_H
#define _SETTINGS_H

extern "C" {
#include <stm32f10x_flash.h>
}

// Need to be completed, check size of page, crc, and so on

class Storage {

friend int main(void);

public:

    struct SettingsData {
        uint32_t var1;
        uint32_t var2;
        uint32_t var3;
        uint32_t var4;
    };

    struct SettingsData data;

    void readToRam(void);
    void writeToFlash(void);

    // delete copy constructor and assignment operator
    Storage(const Storage&) = delete;
    Storage& operator=(const Storage&) = delete;

private:

    Storage();

    const uint32_t MY_FLASH_PAGE_ADDR = 0x800FC00;
    const uint16_t SETTINGS_WORDS = sizeof(data)/4;

};

#endif

