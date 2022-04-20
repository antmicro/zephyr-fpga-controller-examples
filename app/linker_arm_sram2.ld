/*
 * Copyright (c) 2021-2022 Antmicro <www.antmicro.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <autoconf.h>
#include <linker/sections.h>
#include <devicetree.h>

#include <linker/linker-defs.h>
#include <linker/linker-tool.h>

#if defined CONFIG_ARM
  #define CONFIG_DRAM 1
  #define _DRAM_DATA_SECTION_NAME  .DRAM_data
  #define _DRAM_BSS_SECTION_NAME   .DRAM_bss
  #define _DRAM_TEXT_SECTION_NAME  .DRAM_text
  #define DRAM_ADDR  0x3ed00000
#endif

#define RAM_SIZE2 (CONFIG_SRAM_SIZE * 512)
MEMORY
    {
#ifdef CONFIG_DRAM
    DRAM                  (wx) : ORIGIN = 0x3ed00000, LENGTH = 0x10000000
#endif
    }

#include <arch/arm/aarch32/cortex_m/scripts/linker.ld>
