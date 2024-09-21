// Copyright (c) 2024 CarlosFTM
// This code is licensed under MIT license (see LICENSE.txt for details)

/* Define Atomic Register Access
   See section 2.1.3 "Atomic Register Access" on RP2350 datasheet */
#define WRITE_NORMAL (0x0000) // normal read write access
#define WRITE_XOR    (0x1000) // atomic XOR on write
#define WRITE_SET    (0x2000) // atomic bitmask set on write
#define WRITE_CLR    (0x3000) // atomic bitmask clear on write

/* Delay Function
   5 instructions are needed to do a loop. Therefore:
   5 loops @ 12MHz = 2398 clock cycles per ms. */
void delay(unsigned int millisec)
{
    for(unsigned int i = 0; i < millisec * 2398; i++)
    {
        asm("nop");
    }
}

/* Main Function
   Toggle GPIO25 on Pico 2 (on board LED) */
int main() {
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

    while(1)
    {
        // Toggle SIO register to control GPIO25
        (*(volatile unsigned int*)(0xd0000000 + WRITE_SET + 0x028)) = 1 << 25; // xor GPIO (toggle pin)
        delay(100);
    }
    return 0;
}
