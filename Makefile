
#
#	Makefile for hello project
#

DEBUG = -g
CC = qcc
LD = qcc


#TARGET = -Vgcc_ntox86_64
#TARGET = -Vgcc_ntox86
#TARGET = -Vgcc_ntoarmv7le
TARGET = -Vgcc_ntoaarch64le


CFLAGS += $(DEBUG) $(TARGET) -Wall
LDFLAGS+= $(DEBUG) $(TARGET)
BINS = defusee environment
all: $(BINS)

clean:
	rm -f *.o $(BINS);
#	cd solutions; make clean


environment.o: environment.c defs.h
defusee.o: defusee.c defs.h