1.5inch_RGB_oled_demo:OLED_Driver.o OLED_GFX.o main.o
	gcc -Wall -o 1.5inch_RGB_oled_demo OLED_Driver.o OLED_GFX.o main.o -lbcm2835
OLED_Driver.o:OLED_Driver.c OLED_Driver.h
	gcc -Wall -c OLED_Driver.c -lbcm2835
OLED_GFX.o:OLED_GFX.c OLED_GFX.h ASCII_Font.h
	gcc -Wall -c OLED_GFX.c -lbcm2835
main.o:main.c OLED_Driver.h OLED_GFX.h
	gcc -Wall -c main.c -lbcm2835
clean:
	rm main.o OLED_Driver.o OLED_GFX.o 1.5inch_RGB_oled_demo
