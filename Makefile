
#
#	Makefile for hello project
#

DEBUG = -g
CC = qcc
LD = qcc


TARGET = -Vgcc_ntox86_64
#TARGET = -Vgcc_ntox86
#TARGET = -Vgcc_ntoarmv7le
#TARGET = -Vgcc_ntoaarch64le

all:
	qcc -Vgcc_ntoaarch64le  $(TARGET) environment.c -o environment
	qcc -Vgcc_ntoaarch64le	$(TARGET) sensor.c -o sensor
	qcc -Vgcc_ntoaarch64le  $(TARGET) defusee.c -o defusee

clean:
	rm -f *.o environment sensor defusee display
#	cd solutions; make clean


environment.o: environment.c defs.h
defusee.o: defusee.c defs.h
sensor.o: sensor.c defs.h
