#
# Libsensors for allsky camera
# Oleg Kutkov <elenbert@gmail.com>
#

CFLAGS := -fPIC -O2 -Wno-unused-variable -Wall -Werror -I./include -g -ggdb \
	-DUSE_DHT22 -DUSE_TSL_2561 -DUSE_MLX_90614 -DUSE_TACHO -DUSE_RASPI_VCGEN

LDFLAGS = -shared
CC := gcc
RM = rm -f

MAJOR := 0
MINOR := 1
VERSION := $(MAJOR).$(MINOR)

LIBNAME := sensors_allsky
TARGET := lib$(LIBNAME).so.$(VERSION)
TARGET_DIR := /usr/local/lib
TARGET_HEADERS := /usr/local/include

SRCDIR := src

SRCS = $(SRCDIR)/main.c
OBJS = $(SRCS:.c=.o)

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -Wl,-soname,lib$(LIBNAME).so.$(MAJOR) $^ -o $@

.PHONY: install
install:
	cp -f $(TARGET) $(TARGET_DIR)
	ln -sf $(TARGET_DIR)/$(TARGET) $(TARGET_DIR)/lib$(LIBNAME).so
	ln -sf $(TARGET_DIR)/lib$(LIBNAME).so $(TARGET_DIR)/lib$(LIBNAME).so.0
	ldconfig
	
	mkdir -p $(TARGET_HEADERS)/lib$(LIBNAME)
	cp -f include/*.h $(TARGET_HEADERS)/lib$(LIBNAME)

.PHONY: uninstall
uninstall:
	$(RM) $(TARGET_DIR)/lib$(LIBNAME).so*
	$(RM) $(TARGET_DIR)/$(TARGET)
	
	ldconfig
	
	$(RM) -r $(TARGET_HEADERS)/lib$(LIBNAME)

.PHONY: clean
clean:
	$(RM) ${OBJS} $(TARGET)

