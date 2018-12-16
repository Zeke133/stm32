/**
 * @file    bitBanding.hpp
 * @author  Denis Homutovski
 * @version V0.9.0
 * @date    11-12-2018
 * @brief   Bit banding module.
 * @details   Bit banding driver for STM32F10x.
 * @pre       -
 * @bug       -
 * @warning   -
 * @copyright GNU Public License.
 */

#ifndef _BIT_BANDING_H
#define _BIT_BANDING_H

// using
#include <stm32f10x.h>

/**
 * Bit banding implementation.
 * Allow atomic bit read and write operations using
 * ARM Cortex M bit-banding hardware mechanism.
 * There are memory regions of 1MB size to access single bits in
 * SRAM and PERIPHERAL mapped to 32MB words regions.
 */
namespace BitBanding {
    
    /**
     * @brief  Compile-time function to calculate address alias for specified bit.
     * A mapping formula shows how to reference each word in the alias region to a 
     * corresponding bit in the bit-band region. The mapping formula is:
     * bit_word_addr = bit_band_base + (byte_offset x 32) + (bit_number + 4)
     * where:
     * - bit_word_addr: is the address of the word in the alias memory region that 
     *                     maps to the targeted bit.
     * - bit_band_base is the starting address of the alias region
     * - byte_offset is the number of the byte in the bit-band region that contains 
     *     the targeted bit
     * - bit_number is the bit position (0-7) of the targeted bit
     * @param  varAddress: Address of byte to map.
     * @param  bitNumber: Number of needed bit in byte.
     * @retval Address of alias byte to access specified bit.
     */
    constexpr uint8_t * getBitAddress(uint32_t varAddress, uint8_t bitNumber) {

        // static_assert() // compile-time check
        static_assert(SRAM_BASE < PERIPH_BASE, "Wrong memory mapping");
        
        if (varAddress < PERIPH_BASE) {

            // SRAM address mapping
            return (uint8_t *)(SRAM_BB_BASE + (varAddress - SRAM_BASE) * 32 + bitNumber * 4);
        }
        else {

            // Peripheral address mapping
            return (uint8_t *)(PERIPH_BB_BASE + (varAddress - PERIPH_BASE) * 32 + bitNumber * 4);
        }
    };
    
    /**
     * @brief  Function sets specified bit to one.
     * @param  varAddress: Address of byte to map.
     * @param  bitNumber: Number of needed bit in byte.
     * @retval None.
     */
    constexpr void setBit(void * varAddress, uint32_t bitNumber) {
        
        *getBitAddress((uint32_t)varAddress, bitNumber) = 1;
    };

    /**
     * @brief  Function sets specified bit to null.
     * @param  varAddress: Address of byte to map.
     * @param  bitNumber: Number of needed bit in byte.
     * @retval None.
     */
    constexpr void resetBit(void * varAddress, uint8_t bitNumber) {
        
        *getBitAddress((uint32_t)varAddress, bitNumber) = 0;
    };

};

#endif
