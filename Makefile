PRG            = moody
OBJ            = moody.o
MCU_TARGET     = atmega168
MCU						 = atmega168

OPTIMIZE       = -Os

FLASHCMD			= avrdude -b4 -c usbasp -v -p m168 -P usb -U flash:w:$(PRG).hex
ERASECMD      = uisp -dprog=bsd --erase

#SERIAL = /dev/ttyS0
SERIAL = /dev/ttyUSB0


DEFS		= -DF_CPU=16000000
LIBS           =

# You should not have to change anything below here.

CC             = avr-gcc

# Override is only needed by avr-lib build system.

override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override LDFLAGS       = -Wl,-Map,$(PRG).map

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: $(PRG).elf lst text eeprom

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

flash:
#	$(ERASECMD)
#	sleep 1
	$(FLASHCMD)

sflash: $(PRG).hex
	$(LAUNCH_BOOTLOADER) $(SERIAL) 115200
	avrdude -p m8 -b 115200 -u -c avr109 -P $(SERIAL) -U f:w:$(PRG).hex -F

sflasht:
	$(LAUNCH_BOOTLOADER) $(SERIAL) 115200
	avrdude -p m8 -b 115200 -u -c avr109 -P $(SERIAL) -t

lst:  $(PRG).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text: hex bin srec

hex:  $(PRG).hex
bin:  $(PRG).bin
srec: $(PRG).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

# Rules for building the .eeprom rom images

eeprom: ehex ebin esrec

ehex:  $(PRG)_eeprom.hex
ebin:  $(PRG)_eeprom.bin
esrec: $(PRG)_eeprom.srec

%_eeprom.hex: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

%_eeprom.srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@

%_eeprom.bin: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@
