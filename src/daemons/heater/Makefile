#
# Kutkov Oleg (elenbert@gmail.com) (February 2018)
#

CC := gcc
PROGRAM = heater
SRC := heater.c
CFLAGS := -Wall -std=gnu99
LDFLAG := -l bcm2835 -lrt
TARGET_DIR := /opt/allsky/bin

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAG) -o $(PROGRAM)

install:
	mkdir -p $(TARGET_DIR)
	cp $(PROGRAM) $(TARGET_DIR)

clean:
	rm -fr $(PROGRAM) $(PROGRAM).o

