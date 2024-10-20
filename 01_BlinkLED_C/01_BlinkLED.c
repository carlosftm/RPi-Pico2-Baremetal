/* Copyright (c) 2024 CarlosFTM
   SPDX-License-Identifier: GPL-3.0-or-later
   (see LICENSE.txt for details)
*/

#include <stdint.h>

/* Define register access function
   To make the code more readable, we make use of these macros */
#define PUT32(address, value) (*((volatile unsigned int *)(address))) = value
#define GET32(address) (*(volatile unsigned int *)(address))

/* Define Atomic Register Access
   See section 2.1.3 "Atomic Register Access" on RP2350 datasheet */
#define WRITE_NORMAL (0x0000)   // normal read write access
#define WRITE_XOR    (0x1000)   // atomic XOR on write
#define WRITE_SET    (0x2000)   // atomic bitmask set on write
#define WRITE_CLR    (0x3000)   // atomic bitmask clear on write

/* Function declaration */
int main( void );
void Default_Handler(void);
void Reset_Handler(void);

/* Type definitions */
typedef void(*vectors_t)(void);
typedef struct {
    uint32_t word0;
    uint32_t word1;
    uint32_t word2;
    uint32_t word3;
    uint32_t word4;
} PicobinBlockItem;

/* Extern Variables */
extern unsigned int __stack_end__;

/* Vector Table */
__attribute__( ( used, section( ".vector_table" ) ) ) vectors_t vectorTable[] =
{
  (vectors_t)(&__stack_end__), // Initial SP
  Reset_Handler,               // Reset
  Default_Handler,             // NMI
  Default_Handler,             // HardFault
  Default_Handler,             // MemManage
  Default_Handler,             // BusFaults
  Default_Handler,             // UsageFault
  0,                           // Reserved
  0,                           // Reserved
  0,                           // Reserved
  0,                           // Reserved.
  Default_Handler,             // SVCall
  Default_Handler,             // DebugMonitor
  0,                           // Reserved
  Default_Handler,             // PendSV
  Default_Handler,             // SysTick
  Default_Handler,             // IRQ0
  Default_Handler,             // IRQ1 ...
};

/* RP2350 Spec - 5.9.5. Minimum Viable Image Metadata
   As we want to work with ARM Arch, then we use the Minimum Arm IMAGE_DEF
*/
PicobinBlockItem picoBinBlockItem __attribute__((section(".picobin_block_item"))) = {
    .word0 = 0xffffded3,    // PICOBIN_BLOCK_MARKER_START (4 byte magic header)
    .word1 = 0x10210142,    // 0x42 PICOBIN_BLOCK_ITEM_1BS_IMAGE_TYPE, 0x01 word in size, 0x1021 image type exe secure, ARM, RP2350
    .word2 = 0x000001ff,    // 0x00 pad, 0x0001 size, 0xff(size_type == 1, item_type_ == PICOBIN_BLOCK_ITEM_2BS_LAST)
    .word3 = 0x00000000,    // loop containing just this block
    .word4 = 0xab123579,    // PICOBIN_BLOCK_MARKER_END (4 byte magic footer)
};

void Default_Handler(void)
{
  while (1)
  {
    asm("nop");
  }
}

void Reset_Handler(void)
{
  uint32_t cpuId = GET32(0xd0000000);
  if (cpuId == 1)
  {
    // Hold Core1 on infinite loop
    while(1)
    {
        asm("nop");
    }
  }
  main();
}

/* Delay Function
   5 instructions are needed to do a loop. Therefore:
   5 loops @ 12MHz = 2398 clock cycles per ms.
*/
void delay(uint32_t millisec)
{
    for(uint32_t i = 0; i < millisec * 2398; i++)
    {
        asm("nop");
    }
}

/* ConfigDevice Function
   Configures the clock and GPIO
*/
void configDevice(void)
{
    // Setup XOC clock to drive the GPIO (Pico2 board as a ABM8-272-T3 crystal that oscillates at 12MHz)
    (*(volatile unsigned int*)(0x40048000 + 0))      = ( 0x00000aa0);    //  XOC range 1-15MHz (Crystal Oschillator)
    (*(volatile unsigned int*)(0x40048000 + 0x0c))   = ( 0x000000c4);    //  Startup Delay (default = 50,000 cycles aprox.)
    (*(volatile unsigned int*)(0x40048000 + 0x2000)) = ( 0x00FAB000);    //  Enable XOC

    while (!(*(volatile unsigned int*)(0x40048000 + 4)) & ( 1 << 31 ));  //  Wait for XOC stable

    // Configure source clock for components (see datasheer RP2350 Chapter 8. "Clocks")
    (*(volatile unsigned int*)(0x40010000 + 0x3C)) = ( 0 );              //  CLK SYS CTRL = XOC (for processor, bus frabric & memories)

    // Configure GPIO25 to use function 5 (SIO) to controll the GPIO by software
    (*(volatile unsigned int*)(0x40028000 + 0x0cc)) = 5;                 // IO GPIO25 uses SIO

    // Enable GPIO out in SIO register
    (*(volatile unsigned int*)(0xd0000000 + WRITE_SET + 0x038)) = 1 << 25;  // SIO OE (output enable) for Pin25

    // Configure the pad control
    (*(volatile unsigned int*)(0x40038000 + WRITE_CLR + 0x68)) = (1 << 8);  // Remove the pad isolation (new on RP2350)
}

/* -------------
   Main Function
   -------------
*/
int main( void ){
    configDevice();
    while(1)
    {
        // Toggle SIO register to control GPIO25
        (*(volatile unsigned int*)(0xd0000000 + WRITE_SET + 0x028)) = 1 << 25; // xor GPIO (toggle pin)
        delay(50);
    }
    return 0;
}
