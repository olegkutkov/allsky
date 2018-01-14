#
# Kutkov Oleg (kutkov.o@yandex.ru) (July 2016)
#

CPP := g++
PROGRAM = qhy_camera
SRC := src/main.cpp src/qhycam.cpp src/qhy5ii.cpp src/logger.c
CFLAGS := -I./include -Wall -Wno-write-strings  -g -ggdb #-DANNIYING_DEBUG
LDFLAG := -l usb-1.0 -l opencv_highgui -l opencv_core -l opencv_imgproc -l opencv_photo -lcfitsio
TARGET_DIR := /opt/allsky/bin

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CPP) $(CFLAGS) $(SRC) $(LDFLAG) -o $(PROGRAM)

install:
	mkdir -p $(TARGET_DIR)
	cp $(PROGRAM) $(TARGET_DIR)

clean:
	rm -fr $(PROGRAM) $(PROGRAM).o

