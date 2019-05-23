1.5inch_RGB_oled_demo:OLED_Driver.o OLED_GFX.o main.o
	gcc -Wall -o 1.5inch_RGB_oled_demo OLED_Driver.o OLED_GFX.o main.o -lwiringPi
OLED_Driver.o:OLED_Driver.c OLED_Driver.h
	gcc -Wall -c OLED_Driver.c -lwiringPi
OLED_GFX.o:OLED_GFX.c OLED_GFX.h ASCII_Font.h
	gcc -Wall -c OLED_GFX.c -lwiringPi
main.o:main.c OLED_Driver.h OLED_GFX.h
	gcc -Wall -c main.c -lwiringPi
clean:
	rm main.o OLED_Driver.o OLED_GFX.o 1.5inch_RGB_oled_demo
