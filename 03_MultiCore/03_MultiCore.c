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

void delay_core1(uint32_t millisec)
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
    PUT32((0x40048000 + 0),      0x00000aa0);               //  XOC range 1-15MHz (Crystal Oschillator)
    PUT32((0x40048000 + 0x0c),   0x000000c4);               //  Startup Delay (default = 50,000 cycles aprox.)
    PUT32((0x40048000 + 0x2000), 0x00FAB000);               //  Enable XOC
    while (!(GET32(0x40048000 + 4) & ( 1 << 31 )));         //  Wait for XOC stable

    // Configure source clock for components (see datasheer RP2350 Chapter 8. "Clocks")
    PUT32((0x40010000 + 0x3C), 0 );                         //  CLK SYS CTRL = XOC (for processor, bus frabric & memories)
    PUT32((0x40010000 + 0x48), ((1 << 11) | ( 4 << 5)));    //  CLK_PERI_CTRL = XOC (for perifery UART and SPI) + Enable

    // De-asserts the reset of UART0
    PUT32((0x40020000 + WRITE_SET + 0x0), (1 << 26));       // Set UART0 to reset
    asm("nop");
    asm("nop");
    PUT32((0x40020000 + WRITE_CLR + 0x0), (1 << 26));       // De-assert the reset from UART0 
    while (!(GET32(0x40020000 + 0x08) & (1 << 26)));        // Wait for UART0 to be ready

    // Configure GPIO25 to use function 5 (SIO) to controll the GPIO by software
    PUT32((0x40028000 + 0xcc), 5);                          // IO GPIO25 uses SIO
    PUT32((0x40028000 + 0x04), 2);                          // IO GPIO0 uses UART TX
    PUT32((0x40028000 + 0x0c), 2);                          // IO GPIO1 uses UART RX

    // Enable GPIO out in SIO register
    PUT32((0xd0000000 + WRITE_SET + 0x038), (1 << 25));     // SIO OE (output enable) for Pin25

    // Configure the pad control (new on RP2350)
    PUT32((0x40038000 + WRITE_CLR + 0x68), (1 << 8));       // Remove GPIO25 pad isolation
    PUT32((0x40038000 + WRITE_CLR + 0x04), (1 << 8));       // Remove UART0TX pad isolation
    PUT32((0x40038000 + WRITE_CLR + 0x08), (1 << 8));       // Remove UART0RX pad isolation
    PUT32((0x40038000 + WRITE_SET + 0x08), (1 << 6));       // Enable UART0RX pad for input

    // Configure UART0
    //   Baud: For a baud rate of 115200 with UARTCLK = 12MHz then:
    //   Baud Rate Divisor = 12000000/(16 * 115200) ~= 6.5104
    PUT32((0x40070000 + 0x24), 6);                                         // UARTIBRD_H: Integer part of the baudrate divisor
    PUT32((0x40070000 + 0x28), 5104);                                      // UARTFBRD_L: Decimal part of the baudrate divisor
    PUT32((0x40070000 + 0x2c), (( 0x3 << 5 ) | ( 1 << 4 )));               // UARTLCR_H: Word lenght = 8, FIFO RX/TX enabled
    PUT32((0x40070000 + 0x30), ((   1 << 9 ) | ( 1 << 8 ) | ( 1 << 0 )));  // UARTCR: UART Enabled, Tx enabled, Rx enabled

    PUT32((0x40018000 + WRITE_SET + 0), (1 << 24));                        // Take PROC1 out of reset
    while (!(GET32(0x40018000 + 0x0C) & ( 1 << 24 )));                     // Wait until PROC1 is out of reset
}


/* Transmits Character over UART
   Writes a character on the UART TX FIFO
*/
void uartTxChar(int8_t txData)
{
    volatile int8_t myData = txData;
    while(GET32(0x40070000 + 0x018) & (1 << 5));        // Wait until UART0 FIFO is empty
    PUT32((0x40070000 + 0x0), myData);                  // UARTDR: Write data to Tx.
}

/* Transmits string over UART
   Writes a character on the UART TX FIFO
*/
void uartTxString(int8_t* txData)
{
    while(*txData != '\0')
    {
        uartTxChar(*txData++);
    }
    uartTxChar('\r');
    uartTxChar('\n');
}

/* Indicates when data is avaiable on UART Rx FIFO 
*/
int uartRxDataAvail(void)
{
    return ((GET32(0x40070000 + 0x018) & (1 << 4)) == 0);   // Wait until UART0 FIFO is no empty
}

/* Receives character over UART
*/
uint8_t uartRxChar(void)
{
    while(uartRxDataAvail() == 0);                          // Wait until UART0 Rx data is available
    return((unsigned char)GET32(0x40070000 + 0x0));         // UARTDR: Write data to Tx.
}

void main_core_1(void)
{
    while(1)
    {
        delay_core1(100);
        PUT32((0xd0000000 + WRITE_SET + 0x028), (1 << 25));    // xor GPIO (toggle pin)
        delay_core1(5);
        PUT32((0xd0000000 + WRITE_SET + 0x028), (1 << 25));    // xor GPIO (toggle pin)
        delay_core1(1);
        PUT32((0xd0000000 + WRITE_SET + 0x028), (1 << 25));    // xor GPIO (toggle pin)
        delay_core1(5);
        PUT32((0xd0000000 + WRITE_SET + 0x028), (1 << 25));    // xor GPIO (toggle pin)
    }
}

void wait_rx_fifo_data(void)
{
        while ( !( GET32(0xd0000050) & 0x00000001 ) )
        {
            // wait to get data in Rx-FIFO
            asm("wfe");
        }
}

void init_core_1(void)
{
    const uint32_t cmdList[6] = { 0, 0, 1, (int)&vectorTable, ((uint32_t)(&__stack_end__) - 0x100), (int)main_core_1 };
 
    for(int loop = 0; loop < 2; loop++ )
    {
        while( ( GET32( 0xd0000050 ) & 0x01 ) == 1 )  // Loop until Rx FIFO is empty
        {
            GET32( ( 0xd0000000 + 0x58 ) );
        }
        asm( "sev" );
    }

    for (uint8_t i = 0; i < 6; i++)
    {
        PUT32( ( 0xd0000000 + 0x54 ), cmdList[i] );
        asm( "sev" );
        wait_rx_fifo_data();
        uint32_t response = GET32(0xd0000058);
    }
}

/* -------------
   Main Function
   -------------
*/
int main( void )
{
    configDevice();
    init_core_1();
    delay(1000);
    uartTxString((int8_t*)"-= RP2350 MultiCore =-\n\n");

    while(1)
    {
        unsigned char i = '0';
        int8_t textString[] = "[ ] Hola Mundo!";
        while(i <= 'Z')
        {
            textString[1] = i++;
            uartTxString(textString);
            delay(200);
            if (uartRxDataAvail() != 0)
            {
              uartTxChar(uartRxChar());    // Transmit each received character
            }
        }
    }
    return 0;
}
