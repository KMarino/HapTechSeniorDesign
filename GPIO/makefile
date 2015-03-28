
CC=g++

main: main.c GPIO.c GPIO.h /GPIO/dts/*.dts

	chmod 755 dtsScript
	./dtsScript
	$(CC) -o /GPIO/main /GPIO/*.c
	/GPIO/main


.PHONY: main