#ifndef _STORAGE_H
#define _STORAGE_H

extern "C" {
// #include <stm32f1xx_flash.h>
}

/*
ALPHA
Internal flash storage API.
Need to be completed, check size of page, crc, and so on...
*/
class Storage {

public:

    Storage();

    // delete copy constructor and assignment operator
    Storage(const Storage&) = delete;
    Storage& operator=(const Storage&) = delete;

    struct SettingsData {
        uint32_t var1;
        uint32_t var2;
        uint32_t var3;
        uint32_t var4;
    };

    struct SettingsData data;

    void readToRam(void);
    void writeToFlash(void);

private:

    static constexpr uint32_t MY_FLASH_PAGE_ADDR = 0x800FC00;
    static constexpr uint16_t SETTINGS_WORDS = sizeof(data)/4;

};

#endif
