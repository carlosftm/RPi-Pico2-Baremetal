# Copyright (c) 2024 CarlosFTM
# SPDX-License-Identifier: GPL-3.0-or-later
# (see LICENSE.txt for details)
.PHONY: all load loadpt clean

NAME     = 02_BlockingUART
CPU      = cortex-m33
ARMGNU   = arm-none-eabi
OPENOCD  = openocd
OOCDINT  = cmsis-dap#cmsis-dap for picoprobe
PICOTOOL = picotool
FLOAT    = -mfloat-abi=soft#options: ‘soft’, ‘softfp’ and ‘hard’. 
OPTI     = -O0
LDFLAGS  = -mthumb -nostdlib $(FLOAT) --specs=nosys.specs --specs=nano.specs -Wl,-Map=output.map
CFLAGS   = -mcpu=$(CPU) -nostdlib -ffreestanding -nostartfiles -g $(OPTI) -fpic -mthumb -c  $(FLOAT) -mthumb -std=gnu11 -nostdlib --specs=nano.specs

all: $(NAME).elf

$(NAME).o: $(NAME).c
	$(ARMGNU)-gcc $(CFLAGS) $(NAME).c -o $(NAME).o

$(NAME).elf : linker_script.ld $(NAME).o
	$(ARMGNU)-gcc $(LDFLAGS) -T linker_script.ld $(NAME).o -o $(NAME).elf
	objdump -x $(NAME).o > $(NAME).list
	$(PICOTOOL) uf2 convert $(NAME).elf $(NAME).uf2

# Load .elf using openocd
load: $(NAME).elf
	$(OPENOCD) -f interface/$(OOCDINT).cfg -f target/rp2350.cfg -c "adapter speed 5000" -c "program $(NAME).elf verify reset exit" 

# Load .elf using picotool when pico board is in BOOTSEL mode
loadpt: $(NAME).elf
	sudo picotool load $(NAME).elf -x

clean:
	rm -f *.bin *.o *.elf *.list *.map *.uf2
