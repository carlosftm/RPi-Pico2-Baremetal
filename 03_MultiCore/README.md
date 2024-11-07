# 03_MultiCore
With this example, we will learn how to launch code on processor Core 1 (See chapter 5.3 of RP2350 Datasheet).The process to get Core 1 up and running requieres that Core 0 sends to Core 1 a series of values over SIO FIFO for intercore communication. The values include the vector table, stack pointer and the address of the entry function for Core 1.
In this example, Core 0 does the necesary hardware initialization to get the clocks configured, as well as the onboard LED and the UART port. Then it takes Core 1 out of reset and then does necesary procedures to get it operational. Once is all initialization compleated, Core 0 sends and receives data over UART, while Core 1 controls the onboard LED.

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
cd 03_MultiCore
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
    cp 03_MultiCore.uf2 /media/username/RP2350
    ```
