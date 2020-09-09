SDK_HOME = /home/pi/source/github/esp-open-sdk/sdk
CC = $(SDK_HOME)/../xtensa-lx106-elf/bin/xtensa-lx106-elf-gcc
CFLAGS = -I. -mlongcalls -I$(SDK_HOME)/include -I$(SDK_HOME)/driver_lib/driver -I$(SDK_HOME)/driver_lib/include/driver
LDLIBS = -nostdlib -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lpp -lphy -lc -ldriver -lmbedtls -lssl -lcrypto -lpwm -Wl,--end-group -lgcc
LDFLAGS = -T$(SDK_HOME)/ld/eagle.app.v6.ld -L$(SDK_HOME)/lib

pwm-0x00000.bin: pwm
	esptool.py elf2image $^

pwm: pwm.o

pwm.o: pwm.c

flash: pwm-0x00000.bin
	esptool.py write_flash --flash_mode qio 0x0 pwm-0x00000.bin 0x10000 pwm-0x10000.bin

clean:
	rm -f pwm pwm.o pwm-0x00000.bin pwm-0x10000.bin
