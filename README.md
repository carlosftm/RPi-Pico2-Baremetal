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


# Firt time installation of SDK

Link to the PICO SDK reference:
[Raspberry Pi Pico C SDK](https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf)

Link to the RP2350:
[RP2350 Datasheet](https://datasheets.raspberrypi.com/rp2350/rp2350-datasheet.pdf)

## ⚙️ Install the SDK and other tools
I will be using a Ubuntu PC as my development platform. However you can addapt the commands to you specific platform (windows, mac, etc.)

Start by creating a directory called **pico2** on your home directory (~/pico2)
```
mkdir ~/pico2
cd ~/pico2
git clone https://github.com/raspberrypi/pico-sdk
cd pico-sdk
git submodule update --init
cd ..
git clone -b master https://github.com/raspberrypi/pico-examples.git
```
## Clone the *RPi-Pico2-Baremetal* repository
```
cd ~/pico2
git clone https://github.com/carlosftm/Rpi-Pico2-Baremetal.git
```
