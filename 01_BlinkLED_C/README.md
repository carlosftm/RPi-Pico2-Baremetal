# 01_BlinkLED_C
This is the first example for the RP2350. On this example, we will learn how to make a blink LED on C.

In comparison to [RPi-Pico-Baremetal Project](https://github.com/carlosftm/RPi-Pico-Baremetal), all necesary code is implemented in a single .c file. So you will see the `vector tables` and the `Picobin Block Item` definition at the top of the file, as well as the `Reset_Handler()` function that calls the `main()` function.

As a first example, We are using hardcoded register numbers. Later we will make use of the macros defined on the `RP2350.h CMSIS HeaderFile` to make the code more readable and easy to understand.

Most of the instructions on the code are commented, so you can tell what is the purpose of that instruction.

## The code is divided on the following parts:

1. Define of Atomic Register Access: Here we make use of this functionality, where by adding an offset to a register, we can do a set, clear or xor (toggle) of bits on a register.
2. Definition of the `vector tables` and the `Picobin Block Item` tables.
3. Delay function: This a simple delay function that blocks the processing core until the loop is over. The delay time is an aproximation based on the clock frequency and the amount of instructions used by the ARM core.
4. Configuration function:
   - Configures and enabled the XOC clock (crystal oscillator) that oscilates at 12MHz on the Pico 2 board.
   - Configures the clock source for the system clock (clk_sys).
   - Configures the function 5 (SIO) on the GPIO25, so that the GPIO can be controlled by software.
   - Enabled the output of the GPIO
   - Removes the pad isolation. This is an update on the RP2350 compared to the RP2040. An isolation register is added to the pad registers.
5. Main function: In the main function
   - Here basically the GPIO 25 is toggled in a loop after a delay.
  
## Build instructions

To compile this example, we can now do it without the SDK. The `MakeFile` contains the following targets:
- all: Builds the project and generates the .elf and .uf2 files to flash the Pico board. Additionally it generates a .map and .list files to analyze memory sections, and how the functions and variables are allocated in memory.
- load: Use this target to flash the .elf file using openocd with `cmsis-dap` as default interface, which provides support for the pi debug probe as programer/debuger.
- loadpt: If you are not using `pi debug probe`, you can still flash the pico board by getting into `BOOTSEL` mode (plug the pico board with the BOOTSEL button pressed) can calling this target.
- clean: Removes all generated files.

1. Prepare the make files and build directory
```
clone the project: git clone https://github.com/carlosftm/RPi-Pico2-Baremetal.git
cd RPi-Pico2-Baremetal
cd 01_BlinkLED
```
2. Build the project to generate an .elf and .uf2 file
```
make all
```
3. Flash the board:
- using pi debug probe:

    - Attach the pi debug probe to the debug pins of the Pico2 board
    - call:
    ```
    make load
    ```

- via USB and picotool:

    - Connect the board to the USB port of the PC
    - Push and hold the BOOTSEL button while connecting your Pico with a USB cable to a computer.
    - Release the BOOTSEL button once your Pico appears as a Mass Storage Device.
    - Now call:

    ```
    make loadpt
    ```

    or

    ```
    cp 01_BlinkLED.uf2 /media/username/RP2350
    ```