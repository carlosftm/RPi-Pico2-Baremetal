# Welcome to the Raspberry Pico 2 (RP2350) Baremetal Project!
Hey there, fellow tech enthusiasts! 👋

If you loved tinkering with the RP2040 without the official SDK ([RPi-Pico-Baremetal](https://github.com/carlosftm/RPi-Pico-Baremetal)), you're in for a treat! Welcome to the next chapter of our baremetal adventures with the **RP2350**. This repository is your go-to place for diving deep into the world of the RP2350 microcontroller, all while having a blast!

## 🚀 What's This All About?

This project is all about programming the RP2350 microcontroller **without** the official SDK. We're going baremetal, which means we're getting up close and personal with the hardware. Whether you're a seasoned baremetal coder or just curious about what makes these chips tick, there's something here for you.

The unique dual-core, dual-architecture capability of RP2350 provides a pair of industry-standard Arm Cortex-M33 cores, and a pair of open-hardware Hazard3 RISC-V cores, selectable in software or by programming the on-chip OTP memory.

## 🌟 Why RP2350?

The RP2350 is the latest and greatest from the Raspberry Pi Foundation. It's packed with features that make it perfect for a wide range of applications as well as a learning platform for multi-core systems. Here's a sneak peek at what it offers:
- **Blazing Fast Performance**: Dual-Core, dual-architecture with a pair of industry-standard ARM Cortex-M33 cores, and a pair of open-hardware Hazard3 RISC-V cores, the RP2350 is ready to handle your most demanding projects and let us learn both architectures with a single inexpensive development board.
- **Tons of Memory**: 520kB on-chip SRAM and options for external Flash and PSRAM mean you'll never run out of space.
- **Peripherals**: 26 multi-purpose GPIO pins, USB 1.1, 2x UART, 2x SPI, 2x I2C, 24x PWM channels, 12x PIO state machines
- **Fun Extras**: Hardware SHA-256 accelerator

## 📚 What You'll Find Here

- **Code Examples**: Ready-to-run examples to get you started quickly.
- **Documentation**: Detailed guides and tutorials to help you understand every aspect of the RP2350.

## 🚧 Work in Progress

This project is a work in progress, and we need your help to make it awesome! Feel free to reporting issues, or just sharing your thoughts.




# ⚙️ Firt time installation of SDK

Link to the PICO SDK reference:
[Raspberry Pi Pico C SDK](https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf)

Link to the RP2350:
[RP2350 Datasheet](https://datasheets.raspberrypi.com/rp2350/rp2350-datasheet.pdf)

Link to the ´Getting started with Raspberry Pi Pico-series´ for instructions on how to install and build some of the tools to build the projects:
[Getting started](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)

## Install the The GNU Compiler Collection (GCC) and other tools
I will be using a Ubuntu PC as my development platform. However you can addapt the commands to you specific platform (windows, mac, etc.)

Start by creating a directory called **pico2** on your home directory (~/pico2). Then install **The GNU Compiler Collection** and other tools needed to build our projects:
```
mkdir ~/pico2
cd ~/pico2
sudo apt update
sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential
apt install g++ libstdc++-arm-none-eabi-newlib
```
## Build the picotool to flash and to generate the .uf2 files
The .uf2 files are files used to flash the Pico board when it is mounted as hard drive on the PC. The .uf2 file is generated out of the .elf file.
Previously, the ELF-to-UF2 conversion was handled by the elf2uf2 tool in the SDK. Now, for the Pico2 board, the SDK also uses picotool to hash and sign binaries.

if not done already, instal `libusb`:

```
sudo apt install libusb-1.0-0-dev
```

then:

```
cd ~/pico2
git clone https://github.com/raspberrypi/picotool.git
cd picotool
mkdir build
cd build
cmake ..
make -j
```


## Biuld the openocd with support for RP2350/RP2040 
Clone and build OpenOCD with Pico support:
```
git clone https://github.com/raspberrypi/openocd.git
cd openocd
./bootstrap
./configure --disable-werror
make -j
```

or get a binary release from:

```
[https://github.com/raspberrypi/pico-sdk-tools](https://github.com/raspberrypi/pico-sdk-tools)
```

## Clone the *RPi-Pico2-Baremetal* repository
```
cd ~/pico2
git clone https://github.com/carlosftm/Rpi-Pico2-Baremetal.git
```
