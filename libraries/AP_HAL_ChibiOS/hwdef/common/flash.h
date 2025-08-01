

/*
 * Copyright (C) Siddharth Bharat Purohit 2017
 * This file is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "hal.h"

#ifdef __cplusplus
extern "C" {
#endif
uint32_t stm32_flash_getpageaddr(uint32_t page);
uint32_t stm32_flash_getpagesize(uint32_t page);
uint32_t stm32_flash_getnumpages(void);
bool stm32_flash_erasepage(uint32_t page);
bool stm32_flash_write(uint32_t addr, const void *buf, uint32_t count);
void stm32_flash_keep_unlocked(bool set);
bool stm32_flash_ispageerased(uint32_t page);
void stm32_flash_protect_flash(bool bootloader, bool protect);
void stm32_flash_unprotect_flash(void);
void stm32_flash_set_NRST_MODE(uint8_t nrst_mode);
#if defined(STM32H7)
void stm32_flash_corrupt(uint32_t addr, bool double_bit);
#endif
#ifndef HAL_BOOTLOADER_BUILD
bool stm32_flash_recent_erase(void);
#endif
#ifdef __cplusplus
}
#endif
