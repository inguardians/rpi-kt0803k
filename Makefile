rpi-kt0803k: rpi-kt0803k.c rpi-kt0803k.h
	gcc -o rpi-kt0803k rpi-kt0803k.c -O2 -l wiringPi -Wall

.PHONY: install
install:
	install -m 755 rpi-kt0803k /usr/local/bin
