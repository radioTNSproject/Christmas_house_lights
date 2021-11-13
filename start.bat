
avr-gcc -Wall -g -Os -mmcu=attiny85 -o main.bin main.c

avr-objcopy -j .text -j .data -O ihex main.bin main.hex

avrdude -p attiny85 -c USBtiny -U flash:w:main.hex:i -F -P usb -B 250