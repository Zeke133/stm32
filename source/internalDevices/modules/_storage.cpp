#include <_storage.h>

Storage::Storage(void) {

    readToRam();
}

void Storage::readToRam(void) {

    //Read settings
    uint32_t * source_addr = (uint32_t *)MY_FLASH_PAGE_ADDR;
    uint32_t * dest_addr = (uint32_t *)&data;
    for (uint16_t i = 0; i < SETTINGS_WORDS; i++) {
        *dest_addr = *(__IO uint32_t*)source_addr;
        source_addr++;
        dest_addr++;
    }
}

void Storage::writeToFlash(void) {

    FLASH_Unlock();
    FLASH_ErasePage(MY_FLASH_PAGE_ADDR);

    // Write settings
    uint32_t * source_addr = (uint32_t *)&data;
    uint32_t * dest_addr = (uint32_t *) MY_FLASH_PAGE_ADDR;
    for (uint16_t i = 0; i < SETTINGS_WORDS; i++) {
        FLASH_ProgramWord((uint32_t)dest_addr, *source_addr);
        source_addr++;
        dest_addr++;
    }
    FLASH_Lock();
}
