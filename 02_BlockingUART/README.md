# 02_BlockingUART
This example demonstrates how to configure the UART (Universal Asynchronous Receiver/Transmitter) on the RP2350 microcontroller by directly writing to its registers. 
UART is crucial for debugging purposes because it allows developers to send and receive data between the microcontroller and a computer or other devices.
This example uses the UART in blocking mode with FIFO enabled. That means, the microcontroller uses a FIFO to read and write data, but in case the FIFO is full, then the microcontroller needs to wait for the status flags, blocking the microcontroller for other taks.

The UART0 is configured at 115200 bauds, 8 bits, parity none, 1 stop bit, no control flow. The GPIOs used are GPIO0 and GPIO1.

PAy atention to the pad configuration in the RP2350. This is an imrpovement compared to the RP2040. Each GPIO pad needs to be enabled, and in the case of UART_RX GPIO, the pad has to be enabled for input explicitly (default is output enabled).

As serial monitor, I used minicom (Linux) as follow:

```
minicom -D /dev/ttyACM0 -b 115200
```

## Build instructions
1. Prepare the make files and build directory
```
#clone the project: git clone https://github.com/carlosftm/RPi-Pico2-Baremetal.git
cd RPi-Pico2-Baremetal
cd 02_BlockingUART
cp ~/pico2/pico-sdk/external/pico_sdk_import.cmake .
```
2. Build the project to generate an .elf file
```
mkdir build
cd build
export PICO_SDK_PATH=~/pico2/pico-sdk
cmake -DPICO_PLATFORM=rp2350 -DPICO_BOARD=pico2 ..
make -j
```
3. Convert the .elf file to .uf2 to be able to flash the Pico2 board by copying the file when the board is mounted as USB drive on the PC
```
~/pico2/picotool/build/picotool uf2 convert 02_BlockingUART.elf 02_BlockingUART.uf2
```
4. Flash the board
```
#Push and hold the BOOTSEL button while connecting your Pico with a USB cable to a computer.
#Release the BOOTSEL button once your Pico appears as a Mass Storage Device
cp 02_BlockingUART.uf2 /media/username/RP2350
```
