#
# Kutkov Oleg (kutkov.o@yandex.ru) (July 2016)
#

CPP := g++
PROGRAM = qhy_camera
SRC := src/main.cpp src/qhycam.cpp src/qhy5ii.cpp src/fits_handler.cpp src/logger.c
CFLAGS := -I./include -Wall -Wno-write-strings -std=c++11 -g -ggdb #-DANNIYING_DEBUG
LDFLAG := -lusb-1.0 -lcfitsio
TARGET_DIR := /opt/allsky/bin

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CPP) $(CFLAGS) $(SRC) $(LDFLAG) -o $(PROGRAM)

install:
	mkdir -p $(TARGET_DIR)
	cp $(PROGRAM) $(TARGET_DIR)

clean:
	rm -fr $(PROGRAM) $(PROGRAM).o

