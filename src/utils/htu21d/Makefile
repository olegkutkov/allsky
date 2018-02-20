#
# Oleg Kutkov <elenbert@gmail.com> (July 2016)
#

CC := gcc
PROGRAM = read_htu21d
SRC := htu21d.c crc8.c
CFLAGS := -Wall -std=gnu99
TARGET_DIR := /opt/allsky/bin

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAG) -o $(PROGRAM)

install:
	mkdir -p $(TARGET_DIR)
	cp $(PROGRAM) $(TARGET_DIR)

clean:
	rm -fr $(PROGRAM) $(PROGRAM).o
