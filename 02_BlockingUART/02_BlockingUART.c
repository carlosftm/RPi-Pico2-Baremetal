// Copyright (c) 2024 CarlosFTM
// This code is licensed under MIT license (see LICENSE.txt for details)

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

/* ConfigDevice Function
   Configures the clock and GPIO */
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
}

/* Transmits Character over UART
   Writes a character on the UART TX FIFO */
void uartTxChar(unsigned char txData)
{
    volatile unsigned char myData = txData;
    while(GET32(0x40070000 + 0x018) & (1 << 5));        // Wait until UART0 FIFO is empty
    PUT32((0x40070000 + 0x0), myData);                  // UARTDR: Write data to Tx.
}

/* Transmits string over UART
   Writes a character on the UART TX FIFO */
void uartTxString(unsigned char* txData)
{
    while(*txData != '\0')
    {
        uartTxChar(*txData++);
    }
    uartTxChar('\r');
    uartTxChar('\n');
}

/* Indicates when data is avaiable on UART Rx FIFO */
inline int uartRxDataAvail(void)
{
    return ((GET32(0x40070000 + 0x018) & (1 << 4)) == 0);   // Wait until UART0 FIFO is no empty
}

/* Receives character over UART */
char uartRxChar(void)
{
    while(uartRxDataAvail() == 0);                          // Wait until UART0 Rx data is available
    return((unsigned char)GET32(0x40070000 + 0x0));         // UARTDR: Write data to Tx.
}

/* Main Function
   send text on UART0, toggle LED after each line
   and each received character is send back*/
int main() {
    configDevice();
    delay(1000);
    uartTxString("-= UART Blocking Example for RP2350 =-\n\n");

    while(1)
    {
        unsigned char i = '0';
        unsigned char textString[] = "[ ] Hola Mundo!";
        while(i <= 'Z')
        {
            textString[1] = i++;
            uartTxString(textString);
            PUT32((0xd0000000 + WRITE_SET + 0x028), (1 << 25));    // xor GPIO (toggle pin)
            delay(400);
            if (uartRxDataAvail() != 0)
            {
                uartTxChar(uartRxChar());    // Transmit each received character
            }
        }
    }
    return 0;
}

