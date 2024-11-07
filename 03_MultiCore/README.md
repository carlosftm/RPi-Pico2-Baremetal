# 02_BlockingUART
This example demonstrates how to configure the UART (Universal Asynchronous Receiver/Transmitter) on the RP2350 microcontroller by directly writing to its registers. 
UART is crucial for debugging purposes because it allows developers to send and receive data between the microcontroller and a computer or other devices.
This example uses the UART in blocking mode with FIFO enabled. That means, the microcontroller uses a FIFO to read and write data, but in case the FIFO is full, then the microcontroller needs to wait for the status flags, blocking the microcontroller for other taks.

The UART0 is configured at 115200 bauds, 8 bits, parity none, 1 stop bit, no control flow. The GPIOs used are GPIO0 and GPIO1.

Pay atention to the pad configuration in the RP2350. This is an imrpovement compared to the RP2040. Each GPIO pad needs to be enabled, and in the case of UART_RX GPIO, the pad has to be enabled for input explicitly (default is output enabled).

As serial monitor, I used minicom (Linux) as follow:

```
minicom -D /dev/ttyACM0 -b 115200
```

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
cd 02_BlockingUART
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
    cp 02_BlockingUART.uf2 /media/username/RP2350
    ```