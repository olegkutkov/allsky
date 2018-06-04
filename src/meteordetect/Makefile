#
# Kutkov Oleg (elenbert@gmail.com) (June 2018)
#

CPP := g++
PROGRAM = meteor_detector
SRC := src/main.cpp src/fits_handler.cpp
CFLAGS := -I./include -Wall -Wno-write-strings -std=c++11 -g -ggdb
LDFLAG := -l opencv_highgui -l opencv_core -l opencv_imgproc -l opencv_photo -lcfitsio
TARGET_DIR := /opt/allsky/bin

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CPP) $(CFLAGS) $(SRC) $(LDFLAG) -o $(PROGRAM)

install:
	mkdir -p $(TARGET_DIR)
	cp $(PROGRAM) $(TARGET_DIR)

clean:
	rm -fr $(PROGRAM) $(PROGRAM).o

