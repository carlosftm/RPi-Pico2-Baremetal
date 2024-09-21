# 01_BlinkLED_C
This is the first example for the RP2350. On this example, we will learn how to make a blink LED on C.
To compile this example, we will make use of the Raspberry pico-sdk. You may say that this goes against the goal of this project, but currently there is no way around until I'm able to make my own bootloader for the RP2350. I hope this happends soon. But for now, let's focus on how to configure the RP2350 hardware just by writing into registers and not making use of any of the SDK functions.

Most of the instructions on the code are commented, so you can tell what is the purpose of that instruction.

## The code is divided on the following parts:

1. Define of Atomic Register Access: Here we make use of this functionality, where by adding an offset to a register, we can do a set, clear or xor (toggle) of bits on a register.
2. Delay function: This a simple delay function that blocks the processing core until the loop is over. The delay time is an aproximation based on the clock frequency and the amount of instructions used by the ARM core.
3. Configuration function:
   - Configures and enabled the XOC clock (crystal oscillator) that oscilates at 12MHz on the Pico 2 board.
   - Configures the clock source for the system clock (clk_sys).
   - Configures the function 5 (SIO) on the GPIO25, so that the GPIO can be controlled by software.
   - Enabled the output of the GPIO
   - Removes the pad isolation. This is an update on the RP2350 compared to the RP2040. An isolation register is added to the pad registers.
5. Main function: In the main function
   - Here basically the GPIO 25 is toggled in a loop after a delay.
  
## Build instructions
1. Prepare the make files and build directory
```
#clone the project: git clone https://github.com/carlosftm/RPi-Pico2-Baremetal.git
cd RPi-Pico2-Baremetal
cd 01_BlinkLED_C
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
~/pico2/picotool/build/picotool uf2 convert 01_BlinkLED.elf 01_BlinkLED.uf2
```
4. Flash the board
```
#Push and hold the BOOTSEL button while connecting your Pico with a USB cable to a computer.
#Release the BOOTSEL button once your Pico appears as a Mass Storage Device
cp 01_BlinkLED.uf2 /media/username/RP2350
```
