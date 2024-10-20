/* Copyright (c) 2024 CarlosFTM
   SPDX-License-Identifier: GPL-3.0-or-later
   (see LICENSE.txt for details)
*/

/* Linker script for ARM Cortex-M33 */

/* Define memory regions */
MEMORY
{
    FLASH(rx) : ORIGIN = 0x10000000, LENGTH = 4096k
    RAM(rwx)  : ORIGIN = 0x20000000, LENGTH = 512k
}

HEAP_SIZE  = 0x800;   /* 2048 bytes */
STACK_SIZE = 0x400;  /* 1024 bytes */

/* Define entry point */
ENTRY(Reset_Handler)

/* Define sections */
SECTIONS
{
  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    KEEP(*(.vector_table))
    KEEP(*(.picobin_block_item))
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)
  } > FLASH

  /* The initialised data section in RAM
  .data :
  {
    __data_start__ = .;
    *(.data)
    *(.data*)
    __data_end__ = .;
  } >RAM AT> FLASH

  /* Uninitialised data section */
  .bss :
  {
    __bss_start__ = .;
    *(.bss)
    *(.bss*)
    *(COMMON)
    __bss_end__ = .;
  } > RAM

  /* Stack section */
  .stack (NOLOAD) :
  {
    . = ALIGN(8);
    __stack_start__ = .;
    . = . + STACK_SIZE;
    __stack_end__ = .;
  } > RAM

  /* Heap section */
  .heap (NOLOAD) :
  {
    __heap_start__ = .;
    . = . + HEAP_SIZE;
    __heap_end__ = .;
  } > RAM

  _end = .;
}
