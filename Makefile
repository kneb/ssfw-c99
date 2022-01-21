# Name: Makefile
# Author: Neb Konstantin Viktorovich

BUILDDIR = Build
OUTNAME = ssfw

DEVICE = atmega8
CLOCK = 16000000
COMPILER = avr-gcc-5.4.0

ARGS = -mmcu=$(DEVICE) -std=c99 -Os -Wall -fno-exceptions -ffunction-sections -fdata-sections -MMD -flto

LINK = -mmcu=$(DEVICE) -Wall -Wextra -Os -fuse-linker-plugin -Wl,--gc-sections

SRCC := $(wildcard *.c)
OBJECTS := $(patsubst %.c,%.o,$(SRCC))
SRCOBJ := $(addprefix $(BUILDDIR)/, $(OBJECTS))
#SUFF := o -lm
#SRCOBJL := $(patsubst %.o,%.o -lm,$(SRCOBJ))

.PHONY: all clean

.c.o:
	$(COMPILER) $(ARGS) -c $< -o $(BUILDDIR)/$@

all: $(OBJECTS)
	$(COMPILER) $(LINK) $(SRCOBJ) -o $(BUILDDIR)/$(OUTNAME).elf
	@rm -f $(BUILDDIR)/$(OUTNAME).hex
	@avr-objcopy -j .text -j .data -O ihex $(BUILDDIR)/$(OUTNAME).elf $(BUILDDIR)/$(OUTNAME).hex
	@avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex $(BUILDDIR)/$(OUTNAME).elf $(BUILDDIR)/$(OUTNAME)_eep.hex
	@avr-size --format=avr --mcu=$(DEVICE) $(BUILDDIR)/$(OUTNAME).elf
	@echo "Build is Ok $(BUILDDIR)/$(OUTNAME).hex"

size:
	avr-size --format=avr --mcu=$(DEVICE) $(BUILDDIR)/$(OUTNAME).elf
	
clean:
	@rm -f $(BUILDDIR)/*
	@echo "All Clean"

fuse:
	avrdude -c usbasp -p m8 -U lfuse:w:0xFF:m -U hfuse:w:0xD9:m

eeprom:
	avrdude -c usbasp -p m8 -U eeprom:w:$(BUILDDIR)/$(OUTNAME)_eep.hex:i

flash:
	avrdude -c usbasp -p m8 -U flash:w:$(BUILDDIR)/$(OUTNAME).hex:i
