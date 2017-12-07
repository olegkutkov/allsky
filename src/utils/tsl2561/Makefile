#
# Kutkov Oleg (kutkov.o@yandex.ru) (July 2016)
#

CC := gcc
PROGRAM = read_tsl2561
SRC := main.c TSL2561.c 
CFLAGS := -Wall -std=gnu99
LDFLAG :=
TARGET_DIR := /opt/allsky/bin

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAG) -o $(PROGRAM)

install:
	mkdir -p $(TARGET_DIR)
	cp $(PROGRAM) $(TARGET_DIR)

clean:
	rm -fr $(PROGRAM) $(PROGRAM).o
