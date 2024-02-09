## Application settings

SOURCES = $(wildcard *.c)
HEADERS = 
TARGETS = $(foreach s,$(SOURCES),$(notdir $(basename $s)))

INCLUDES = -I./../ -I./../../../../
LIBS =

## Compiler settings

CROSS_COMPILE = arm-unknown-linux-uclibcgnueabi-
CC := $(CROSS_COMPILE)gcc
STRIP = $(CROSS_COMPILE)strip
SIZE = $(CROSS_COMPILE)size
CFLAGS = -O2 -Wall -Werror
LDFLAGS = -lpthread

## Execute build

all: $(TARGETS)

$(TARGETS): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $@.c $(INCLUDES) $(LIBS) $(LDFLAGS)

clean:
	@rm -rf $(TARGETS)

install:
	scp $(TARGETS) root@$(BOARD):/home/$(USER)

.PHONY: default clean install
